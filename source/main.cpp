#include "bn_core.h"
#include "bn_bg_palettes.h"

#include "ui/room_screen.h"
#include "engine/rng.h"
#include "engine/save_game.h"

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

    // Phase 4g's title screen: New Game always starts fresh at
    // ForestCrossing (every room/GameState/Inventory singleton is already
    // at its just-constructed default — nothing to reset); Continue is
    // only offered, and only actually loads SRAM, if a save exists.
    bool has_save = sg::SaveGame::save_exists();
    int picked = screen.show_title_screen(has_save);
    sg::RoomId start = sg::RoomId::ForestCrossing;

    if(has_save && picked == 1)
    {
        start = sg::SaveGame::load_or_new_game();
    }
    else
    {
        screen.show_intro();
    }

    // The stats/item sidebars (including the character portrait) stay
    // hidden through the title screen and intro — revealed here, right
    // before real gameplay's first room entry.
    screen.set_sidebars_visible(true);

    screen.run(start);
}
