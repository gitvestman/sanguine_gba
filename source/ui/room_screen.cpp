#include "room_screen.h"

#include "bn_core.h"
#include "bn_string.h"

#include "common_variable_8x8_sprite_font.h"

#include "../engine/dice.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/save_game.h"

namespace sg
{

namespace
{
    // Each stat is one line (2-letter code, left-aligned + number,
    // right-aligned) instead of two, so all seven fit in the panel's bottom
    // half below the portrait. Also fills in a flash color per line by
    // comparing against prev_stats (one entry per stat, updated in place) —
    // -1 means "not drawn yet", so the very first draw never flashes.
    void add_stat_line(bn::ivector<bn::string<4>>& codes, bn::ivector<bn::string<4>>& values,
                        bn::ivector<ui::StatFlash>& flashes, int& prev_value, const char* code, int value)
    {
        codes.push_back(bn::string<4>(code));
        values.push_back(bn::to_string<4>(value));

        if(prev_value < 0 || prev_value == value)
        {
            flashes.push_back(ui::StatFlash::NONE);
        }
        else
        {
            flashes.push_back(value > prev_value ? ui::StatFlash::GAIN : ui::StatFlash::LOSS);
        }

        prev_value = value;
    }

    void build_stat_lines(bn::ivector<bn::string<4>>& codes, bn::ivector<bn::string<4>>& values,
                           bn::ivector<ui::StatFlash>& flashes, int (&prev_stats)[7])
    {
        GameState& gs = GameState::instance();

        add_stat_line(codes, values, flashes, prev_stats[0], "HP", gs.stat(Attribute::HP));
        add_stat_line(codes, values, flashes, prev_stats[1], "SN", gs.stat(Attribute::SANITY));
        add_stat_line(codes, values, flashes, prev_stats[2], "LK", gs.stat(Attribute::LUCK));
        add_stat_line(codes, values, flashes, prev_stats[3], "IN", gs.stat(Attribute::INT_));
        add_stat_line(codes, values, flashes, prev_stats[4], "ST", gs.stat(Attribute::STR));
        add_stat_line(codes, values, flashes, prev_stats[5], "DX", gs.stat(Attribute::DEX));
        add_stat_line(codes, values, flashes, prev_stats[6], "DC", gs.stat(Attribute::DC));
    }

    void build_owned_items(bn::ivector<ItemId>& out)
    {
        Inventory& inventory = Inventory::instance();

        for(int i = 0; i < int(ItemId::COUNT) && ! out.full(); ++i)
        {
            if(inventory.has(ItemId(i)))
            {
                out.push_back(ItemId(i));
            }
        }
    }
}

RoomScreen::RoomScreen() :
    _text_generator(common::variable_8x8_sprite_font),
    _text_box(_text_generator),
    _choices(_text_generator),
    _item_bar(-104),
    _stats_panel(_text_generator, 104)
{
}

// show_text and choose redraw the sidebars first, since stats and items
// only change in response to an action, and every action's outcome is shown
// through one of these two. Checking for game over here too means it's
// caught right after whatever action killed the player, regardless of
// where that damage came from.
void RoomScreen::show_text(const bn::string_view& text)
{
    _refresh_sidebars();
    _check_game_over();
    _text_box.show(text);

    while(_text_box.active())
    {
        _text_box.update();
        bn::core::update();
    }
}

int RoomScreen::choose(const bn::ivector<bn::string<40>>& options)
{
    _refresh_sidebars();
    _check_game_over();
    _choices.set_options(options);

    while(true)
    {
        bn::optional<int> picked = _choices.update();

        // Always update before returning (even on the frame a pick is
        // detected), matching show_text()'s loop shape below — otherwise
        // Butano's "A just pressed" flag (which only refreshes on
        // bn::core::update(), see bn_keypad_manager.cpp) is still set when
        // we return, and whatever runs next (almost always a show_text()
        // call) sees that same stale press and immediately auto-advances
        // or dismisses its own first page before it's ever actually shown
        // on screen.
        bn::core::update();

        if(picked.has_value())
        {
            return picked.value();
        }
    }
}

Result RoomScreen::roll(Attribute attribute, int dc, bool cancelable, const bn::string_view& title)
{
    return Dice::roll_check(*this, attribute, dc, cancelable, title);
}

void RoomScreen::tumble_die(int value)
{
    _refresh_sidebars();
    _dice_view.tumble_to(value);
}

void RoomScreen::hide_die()
{
    _dice_view.hide();
}

void RoomScreen::show_enemy(EnemyId id)
{
    show_overlay_sprite(enemy_sprite_item(id));
}

void RoomScreen::hide_enemy()
{
    hide_overlay_sprite();
}

void RoomScreen::show_overlay_sprite(const bn::sprite_item& item)
{
    _enemy_sprite = item.create_sprite(0, -8);
}

void RoomScreen::hide_overlay_sprite()
{
    _enemy_sprite.reset();
}

void RoomScreen::goto_room(RoomId next, const bn::string_view& transition_text)
{
    if(! transition_text.empty())
    {
        show_text(transition_text);
    }

    _enter_room(next);
}

void RoomScreen::_check_game_over()
{
    if(_game_over || ! GameState::instance().is_dead())
    {
        return;
    }

    _game_over = true;
    _dice_view.hide();
    _text_box.show("You succumb to your wounds.\n\nGAME OVER");

    // Deliberately never call _text_box.update(): that's what lets an A
    // press advance/dismiss a page, and this message should stay on screen
    // for good — there's no death screen or restart yet (see the class
    // comment on _check_game_over).
    while(true)
    {
        bn::core::update();
    }
}

void RoomScreen::_refresh_sidebars()
{
    bn::vector<bn::string<4>, ui::SidePanel::max_lines> stat_codes;
    bn::vector<bn::string<4>, ui::SidePanel::max_lines> stat_values;
    bn::vector<ui::StatFlash, ui::SidePanel::max_lines> stat_flashes;
    build_stat_lines(stat_codes, stat_values, stat_flashes, _prev_stats);
    _stats_panel.set_lines(stat_codes, stat_values, stat_flashes);

    bn::vector<ItemId, ui::ItemBar::max_icons> items;
    build_owned_items(items);
    _item_bar.set_items(items);
}

void RoomScreen::_enter_room(RoomId id)
{
    _current_room = id;

    // Defensive: every Combat::fight() exit path already hides its enemy
    // sprite, but a room change is the natural place to guarantee no stray
    // sprite ever survives past its fight.
    _enemy_sprite.reset();

    // The GBA has a single 256-color background palette, and Butano reuses
    // an already-loaded 8bpp palette for a new background without checking
    // its colors (palettes_bank::find_bpp_8). Releasing the previous room's
    // background first is what makes the new room upload its own palette
    // instead of being drawn with the old room's colors.
    _background.reset();

    // Shifts the room art so its own top edge lands exactly on the
    // screen's top edge (canvas row 58, where the art starts within its
    // 256x256 canvas — see tools/convert_room_bg.py) instead of centering
    // it, so no art is cropped at the top; the bottom panel covers the
    // bottom of the art and the border below it.
    _background = background_item(id).create_bg(0, -10);

    Room& r = room(id);
    bn::string<768> description;
    r.describe(description);
    show_text(description);
    r.on_enter(*this);

    // Autosave on every room change (gamestate_singleton.gd's autosave(),
    // called from its own enter_room()) — since combat never changes rooms
    // while it's in progress, this also means a mid-fight death never
    // overwrites the checkpoint from before that fight started.
    SaveGame::save(id);
}

void RoomScreen::run(RoomId start)
{
    _enter_room(start);

    while(true)
    {
        Room& r = room(_current_room);

        bn::vector<Choice, 8> choices;
        r.build_choices(choices);

        bn::vector<bn::string<40>, 8> labels;
        for(const Choice& choice : choices)
        {
            labels.push_back(choice.text);
        }

        int picked = choose(labels);
        r.on_choice(choices[picked].action_id, *this);
    }
}

}
