#include "room_screen.h"

#include "bn_core.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_algorithm.h"
#include "bn_math.h"
#include "bn_utility.h"

#include "common_variable_8x8_sprite_font.h"

#include "../engine/dice.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/save_game.h"
#include "../engine/escape.h"
#include "../engine/combat.h"
#include "../engine/item_info.h"

#include "bn_sprite_items_panel_tile.h"
#include "bn_sprite_items_ambient_torch.h"
#include "bn_sprite_items_ambient_candle.h"
#include "bn_regular_bg_items_castle_title.h"
#include "bn_regular_bg_items_castle_menu.h"
#include "bn_regular_bg_items_intro_trees_far.h"
#include "bn_regular_bg_items_intro_trees_mid.h"
#include "bn_regular_bg_items_intro_trees_near.h"

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
        _update_enemy_animation();
        _update_ambient();
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
        _update_enemy_animation();
        _update_ambient();

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

namespace
{
    // Approximates rat_floor()'s six-waypoint dash loop (utils/sprite_animations.gd),
    // its 180px-wide canvas x's rescaled onto this overlay's much narrower safe
    // travel range (see the class comment on _update_enemy_animation).
    constexpr bn::fixed rat_waypoints_x[6] = { 44, 4, 52, -48, -11, -37 };
    constexpr int rat_pause_frames = 21;
    constexpr bn::fixed rat_step = 1;

    constexpr bn::fixed figure_eight_amplitude_x = 40;
    constexpr bn::fixed figure_eight_amplitude_y = 20;
    const bn::fixed figure_eight_step_degrees = 1.5;
}

void RoomScreen::show_enemy(EnemyId id)
{
    show_overlay_sprite(enemy_sprite_item(id));

    _enemy_anim = enemy_anim_kind(id);
    _anim_frame = 0;
    _anim_index = 0;
    _anim_paused = false;

    switch(_enemy_anim)
    {

    case EnemyAnim::RAT_FLOOR:
        _enemy_sprite->set_scale(2);
        _anim_x = rat_waypoints_x[5];
        _enemy_sprite->set_x(_anim_x);
        _enemy_sprite->set_horizontal_flip(rat_waypoints_x[0] < _anim_x);
        break;

    case EnemyAnim::FIGURE_EIGHT:
        _anim_x = 0;
        break;

    case EnemyAnim::NONE:
        break;
    }
}

void RoomScreen::hide_enemy()
{
    hide_overlay_sprite();
}

void RoomScreen::show_overlay_sprite(const bn::sprite_item& item)
{
    _enemy_sprite = item.create_sprite(0, -8);
    _enemy_anim = EnemyAnim::NONE;
}

void RoomScreen::hide_overlay_sprite()
{
    _enemy_sprite.reset();
    _enemy_anim = EnemyAnim::NONE;
}

void RoomScreen::_update_enemy_animation()
{
    if(! _enemy_sprite.has_value())
    {
        return;
    }

    switch(_enemy_anim)
    {

    case EnemyAnim::RAT_FLOOR:
    {
        bn::fixed target = rat_waypoints_x[_anim_index];

        if(_anim_paused)
        {
            ++_anim_frame;

            if(_anim_frame >= rat_pause_frames)
            {
                _anim_paused = false;
                _anim_frame = 0;
                _anim_index = (_anim_index + 1) % 6;
                _enemy_sprite->set_horizontal_flip(rat_waypoints_x[_anim_index] < _anim_x);
            }

            return;
        }

        if(_anim_x < target)
        {
            _anim_x = bn::min(_anim_x + rat_step, target);
        }
        else if(_anim_x > target)
        {
            _anim_x = bn::max(_anim_x - rat_step, target);
        }

        _enemy_sprite->set_x(_anim_x);

        if(_anim_x == target)
        {
            _anim_paused = true;
            _anim_frame = 0;
        }

        break;
    }

    case EnemyAnim::FIGURE_EIGHT:
    {
        ++_anim_frame;

        bn::fixed angle = bn::fixed(_anim_frame) * figure_eight_step_degrees;
        bn::fixed x = figure_eight_amplitude_x * bn::degrees_sin(angle);
        bn::fixed y = -8 + figure_eight_amplitude_y * bn::degrees_sin(angle * 2);
        _enemy_sprite->set_position(x, y);
        break;
    }

    case EnemyAnim::NONE:
        break;
    }
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

    if(! _sidebars_visible)
    {
        _stats_panel.set_visible(false);
        _item_bar.set_visible(false);
    }
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
    _refresh_ambient(id);

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

void RoomScreen::_refresh_ambient(RoomId id)
{
    _ambient.clear();

    bn::vector<AmbientDecor, max_ambient> decor;
    room(id).build_ambient(decor);

    for(const AmbientDecor& d : decor)
    {
        const bn::sprite_item& item = d.kind == AmbientKind::TORCH
                ? bn::sprite_items::ambient_torch : bn::sprite_items::ambient_candle;
        AmbientSprite entry{ item.create_sprite(d.x, d.y), d.kind };
        entry.sprite.set_z_order(1);
        _ambient.push_back(bn::move(entry));
    }
}

void RoomScreen::_update_ambient()
{
    // Matches the source's SpriteFrames "speed 5.0" (5 frames/second) at
    // this engine's 60 FPS.
    constexpr int wait_updates = 12;

    for(AmbientSprite& entry : _ambient)
    {
        ++entry.wait;

        if(entry.wait < wait_updates)
        {
            continue;
        }

        entry.wait = 0;

        int frame_count = entry.kind == AmbientKind::TORCH ? 8 : 6;
        entry.frame = (entry.frame + 1) % frame_count;

        const bn::sprite_item& item = entry.kind == AmbientKind::TORCH
                ? bn::sprite_items::ambient_torch : bn::sprite_items::ambient_candle;
        entry.sprite.set_tiles(item.tiles_item(), entry.frame);
    }
}

void RoomScreen::set_sidebars_visible(bool visible)
{
    _sidebars_visible = visible;
    _stats_panel.set_visible(visible);
    _item_bar.set_visible(visible);
    _panel.set_visible(visible);
}

int RoomScreen::_choose_centered(const bn::ivector<bn::string<40>>& options)
{
    int selected = 0;
    bn::vector<bn::sprite_ptr, 8 * 4> sprites;

    auto regenerate = [&]
    {
        sprites.clear();
        _text_generator.set_center_alignment();

        bn::fixed top_y = -8 * bn::fixed(options.size() - 1);

        for(int i = 0; i < options.size(); ++i)
        {
            bn::string<44> line = (i == selected) ? bn::string<44>("> ") : bn::string<44>("  ");
            line.append(options[i]);
            _text_generator.generate(0, top_y + (16 * i), line, sprites);
        }
    };

    regenerate();

    while(true)
    {
        bool changed = false;

        if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % options.size();
            changed = true;
        }
        else if(bn::keypad::up_pressed())
        {
            selected = (selected - 1 + options.size()) % options.size();
            changed = true;
        }

        if(changed)
        {
            regenerate();
        }

        bn::core::update();

        if(bn::keypad::a_pressed())
        {
            // One more update() before returning so this frame's "A just
            // pressed" edge is consumed here rather than surviving into
            // whatever runs next — same reasoning as choose()'s own comment
            // on this: without it, the very next input-reading loop (e.g.
            // show_intro()'s TextBox) sees this same stale press on its
            // first check, before it has ever called update() itself, and
            // instantly acts on it (observed as the intro's first page
            // flashing by unseen, since a fresh New Game selection landed
            // here with A still "just pressed").
            bn::core::update();
            return selected;
        }
    }
}

void RoomScreen::_wait_for_a(const bn::string_view& text, bn::fixed y)
{
    bn::vector<bn::sprite_ptr, 16> sprites;
    _text_generator.set_center_alignment();
    _text_generator.generate(0, y, text, sprites);

    // bn::core::update() must run every iteration, including the one where
    // it turns out A was pressed (see room_screen.cpp's stale-input lesson
    // from Phase 1 — checking a_pressed() before ever calling update() this
    // frame would read whatever press was still latched from before this
    // screen even started).
    while(true)
    {
        bn::core::update();

        if(bn::keypad::a_pressed())
        {
            break;
        }
    }
}

int RoomScreen::show_title_screen(bool has_save)
{
    set_sidebars_visible(false);

    _background.reset();
    _background = bn::regular_bg_items::castle_title.create_bg(0, 0);
    _wait_for_a("Press A to begin", -50);

    _background.reset();
    _background = bn::regular_bg_items::castle_menu.create_bg(0, 0);

    bn::vector<bn::string<40>, 2> labels;
    labels.push_back(bn::string<40>("New Game"));

    if(has_save)
    {
        labels.push_back(bn::string<40>("Continue"));
    }

    return _choose_centered(labels);
}

void RoomScreen::show_intro()
{
    // A bigger local backdrop + text box just for the intro's story text
    // (9 lines over a 96px band, instead of the standard 3 over 32px) —
    // the shared _panel/_text_box stay hidden throughout, since there's no
    // room art to preserve here and a taller box reads more of each page
    // at once, closer to the source project's own longer intro prose.
    bn::vector<bn::sprite_ptr, 12> intro_panel_tiles;
    constexpr bn::fixed intro_panel_xs[] = { -96, -32, 32, 96 };
    constexpr bn::fixed intro_panel_ys[] = { 0, 32, 64 };

    for(bn::fixed y : intro_panel_ys)
    {
        for(bn::fixed x : intro_panel_xs)
        {
            intro_panel_tiles.push_back(bn::sprite_items::panel_tile.create_sprite(x, y));
        }
    }

    ui::TextBox intro_text_box(_text_generator, 9, -10);

    // Background art (castle_menu, already showing from show_title_screen)
    // stays as the backdrop; the tree layers scroll in front of it in
    // increasing priority (lower number = drawn later = closer to the
    // viewer — see bn_regular_bg_ptr::set_priority) for a parallax depth
    // effect, matching the source project's three tree_lagerNN layers.
    if(_background)
    {
        _background->set_priority(3);
    }

    // Positioned just above the intro's own (now taller) text backdrop
    // (screen y:[-16,80) — see intro_panel_ys above), each layer's bottom
    // edge landing a few px shy of that boundary so the nearest (tallest)
    // layer doesn't dip down behind it.
    bn::regular_bg_ptr trees_far = bn::regular_bg_items::intro_trees_far.create_bg(0, -36);
    bn::regular_bg_ptr trees_mid = bn::regular_bg_items::intro_trees_mid.create_bg(0, -44);
    bn::regular_bg_ptr trees_near = bn::regular_bg_items::intro_trees_near.create_bg(0, -52);
    trees_far.set_priority(2);
    trees_mid.set_priority(1);
    trees_near.set_priority(0);

    // Pages 1 and 3 are adapted from the source project's own coach-ride
    // narration (CHARACTER_SELECT_DESCRIPTION and
    // CHARACTER_SELECT_COACH_STOP_DESCRIPTION in localization/sanguine_en.po)
    // — the waking-up-alone beat and the coachman stopping short of the
    // castle — condensed to this build's terser style and with the source's
    // character-select and bag-item steps (no such systems exist here, one
    // character is hardcoded) left out entirely. Page 2 is original
    // atmosphere carried over from this port's first intro pass.
    const bn::string_view pages[] = {
        "You awake with a start as the coach rattles over rough stone, deep in the "
        "mountains. The other passengers are gone; you don't recall them leaving. Still, "
        "it must be the 5th of September, 1846.\n\n"
        "In the fading light, your own reflection stares back from the window glass, "
        "dark forest and jagged peaks rising behind it.",
        "The horses grow skittish long before the castle comes into view, as if the road "
        "itself remembers what waits ahead. Somewhere out past the treeline, something answers "
        "the wind that no wind should.\n\n"
        "You grip whatever you brought to protect yourself, and try not to think too hard "
        "about why you came.",
        "The coach jerks to a stop. \"I cannot drive closer,\" the driver calls back in a "
        "thick accent, \"but the road to the castle, it is just ahead, yes?\"\n\n"
        "You step out alone, and the coach rattles away into the dark. A narrow dirt path "
        "winds off from the road, vanishing into black pine forest."
    };

    bn::fixed far_x;
    bn::fixed mid_x;
    bn::fixed near_x;

    for(const bn::string_view& page : pages)
    {
        intro_text_box.show(page);

        while(intro_text_box.active())
        {
            far_x -= 0.05;
            mid_x -= 0.125;
            near_x -= 0.25;
            trees_far.set_x(far_x);
            trees_mid.set_x(mid_x);
            trees_near.set_x(near_x);

            intro_text_box.update();
            bn::core::update();
        }
    }
}

void RoomScreen::_open_inventory()
{
    // Every owned item, one page of up to page_size at a time (plus "Back",
    // and "More" when another page follows) — kept below
    // ui::ChoiceList::max_options even if every item in the game is owned
    // at once, unlike a single flat list of all ItemId::COUNT entries.
    constexpr int page_size = ui::ChoiceList::max_options - 2;
    int page = 0;

    while(true)
    {
        bn::vector<ItemId, int(ItemId::COUNT)> items;

        for(int i = 0; i < int(ItemId::COUNT); ++i)
        {
            if(Inventory::instance().has(ItemId(i)))
            {
                items.push_back(ItemId(i));
            }
        }

        if(items.empty())
        {
            show_text("Your bag is empty.");
            return;
        }

        int page_count = (items.size() + page_size - 1) / page_size;

        if(page >= page_count)
        {
            page = 0;
        }

        int start = page * page_size;
        int end = bn::min(start + page_size, int(items.size()));
        bool more = end < int(items.size());

        bn::vector<bn::string<40>, ui::ChoiceList::max_options> labels;
        for(int i = start; i < end; ++i)
        {
            labels.push_back(bn::string<40>(item_info(items[i]).name));
        }

        if(more)
        {
            labels.push_back("More");
        }

        labels.push_back("Back");

        int picked = choose(labels);

        if(picked == labels.size() - 1)
        {
            return;
        }

        if(more && picked == labels.size() - 2)
        {
            ++page;
            continue;
        }

        ItemId item = items[start + picked];
        show_text(item_info(item).description);

        if(! item_is_usable(item))
        {
            continue;
        }

        bn::vector<bn::string<40>, 2> use_labels;
        use_labels.push_back("Use");
        use_labels.push_back("Back");

        if(choose(use_labels) == 0)
        {
            use_item(item, *this);
        }
    }
}

void RoomScreen::run(RoomId start)
{
    _enter_room(start);

    while(true)
    {
        Room& r = room(_current_room);

        bn::vector<Choice, ui::ChoiceList::max_options - 1> choices;
        r.build_choices(choices);

        bn::vector<bn::string<40>, ui::ChoiceList::max_options> labels;
        for(const Choice& choice : choices)
        {
            labels.push_back(choice.text);
        }
        labels.push_back("Inv");

        int picked = choose(labels);

        if(picked == choices.size())
        {
            _open_inventory();
            continue;
        }

        RoomId room_before = _current_room;
        r.on_choice(choices[picked].action_id, *this);
        _handle_escape_pursuit(_current_room != room_before);
    }
}

void RoomScreen::win()
{
    _refresh_sidebars();
    _text_box.show("You reach the coach and flee into the night, leaving the castle and "
                    "its horrors behind you.\n\nYOU ESCAPED");

    // Deliberately never call _text_box.update() (see _check_game_over) —
    // there's no post-game menu yet to return to.
    while(true)
    {
        bn::core::update();
    }
}

void RoomScreen::_handle_escape_pursuit(bool moved)
{
    if(! is_escaping())
    {
        _escape_pursuit = 0;
        return;
    }

    if(moved)
    {
        _escape_pursuit = 0;
        return;
    }

    ++_escape_pursuit;

    if(_escape_pursuit == 1)
    {
        show_text("You hear footsteps behind you in the dark.");
        return;
    }

    _escape_pursuit = 0;
    bn::optional<EnemyId> pursuer = pick_pursuer();

    if(! pursuer.has_value())
    {
        return;
    }

    Enemy& target = enemy(pursuer.value());
    bn::string<160> text;
    text.append(target.name);
    text.append(" catches up to you!");
    show_text(text);
    Combat::fight(*this, pursuer.value());
}

}
