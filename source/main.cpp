#include "bn_core.h"
#include "bn_bg_palettes.h"

#include "ui/room_screen.h"
#include "engine/rng.h"
#include "engine/save_game.h"
#include "engine/settings.h"
#include "engine/inventory.h"
#include "engine/music.h"

int main()
{
    bn::core::init();

    // The GBA has no hardware random source and bn::random starts from a
    // fixed seed, so without this every playthrough rolls the same numbers.
    // Advancing it every frame makes each roll depend on the exact frame
    // the player pressed a button on.
    bn::core::set_update_callback([] { sg::rng().update(); });

    // Background palette index 0 is always drawn as this backdrop color.
    // tools/convert_room_bg.py puts each room's near-black border fill on
    // index 0, so this matches it.
    bn::bg_palettes::set_transparent_color(bn::color(1, 1, 1));

    // RoomScreen holds the background, text box, choice list, both
    // sidebars and the dice view — a few KB. As a plain local here, that
    // entire block would sit on the stack for main()'s whole lifetime
    // (main never returns while the game runs), eating into the stack
    // headroom every deeper call (combat -> dice roll -> text/choice
    // updates) needs. `static` moves it out of the stack; BN_DATA_EWRAM
    // additionally moves it from IWRAM (32KB, shared with every room's
    // own static state — see room_registry.cpp) into EWRAM (256KB), which
    // has room to spare.
    BN_DATA_EWRAM static sg::RoomScreen screen;

    // Restores music/SFX volume from SRAM (or leaves both at max, the
    // just-constructed default, on a first-ever boot) and pushes them to
    // bn::music/bn::sound right away, before anything else can play.
    sg::Settings::instance().load();

    // Phase 4g's title screen, now with 3 save slots (matching the source's
    // MAX_SAVE_SLOTS): New Game always starts fresh at OutsideGates — the
    // coach-ride intro's actual destination, same as the source project
    // (every room/GameState/Inventory singleton is already at its
    // just-constructed default — nothing to reset); Continue is only
    // offered, and its slot picker only lists occupied slots, if any slot
    // has a save. The "press A" splash shows once; backing out of the
    // slot picker with B loops back to just the New Game/Continue menu,
    // not the splash again.
    screen.show_title_card();

    sg::RoomId start = sg::RoomId::OutsideGates;

    while(true)
    {
        bool has_save = sg::SaveGame::any_save_exists();
        int picked = screen.choose_new_or_continue(has_save);

        if(picked == 2)
        {
            screen.show_settings_menu();
            continue;
        }

        if(has_save && picked == 1)
        {
            int slot = screen.choose_save_slot(true);

            if(slot < 0)
            {
                continue;
            }

            start = sg::SaveGame::load_game(slot);
            break;
        }

        int slot = screen.choose_save_slot(false);

        if(slot < 0)
        {
            continue;
        }

        sg::SaveGame::start_new_game(slot);

        // Matches the source project's starting bag (character_select_
        // room.gd's CHARACTER_SELECT_FLINTLOCK_DESCRIPTION) — every
        // character in this build carries the pistol, since there's no
        // character-select step here to choose a different starting item.
        sg::Inventory::instance().add(sg::ItemId::FLINTLOCK);
        screen.show_intro();
        break;
    }

    // The stats/item sidebars (including the character portrait) stay
    // hidden through the title screen and intro — revealed here, right
    // before real gameplay's first room entry.
    screen.set_sidebars_visible(true);

    // The title theme (started in show_title_card()) hands off to the
    // default in-game ambient track here, right as real gameplay begins —
    // matches the source's GameState.change_state(PLAY) fading intro_music
    // into game_music.
    sg::play_default_theme();

    screen.run(start);
}
