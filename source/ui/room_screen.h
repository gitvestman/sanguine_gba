#ifndef SG_UI_ROOM_SCREEN_H
#define SG_UI_ROOM_SCREEN_H

#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "panel.h"
#include "text_box.h"
#include "choice_list.h"
#include "side_panel.h"
#include "item_bar.h"
#include "dice_view.h"
#include "../engine/room.h"
#include "../engine/constants.h"
#include "../engine/enemy.h"

// The "game.gd" equivalent: owns the background + text box + choice list and
// drives the enter-room / show-description / show-choices / act-on-choice
// loop for the whole session. Everything here blocks (its own
// bn::core::update() loops) rather than being driven by a state machine
// ticked from main(), matching the "blocking scene function" shape Butano's
// own examples use — there is nothing else animating concurrently in this
// turn-based game that would need a non-blocking loop.
namespace sg
{

class RoomScreen
{

public:
    RoomScreen();

    // Phase 4g's title card: the castle art with a "press A" prompt, then
    // switches the background to the plain castle menu art underneath the
    // New Game/Continue menu (see choose_new_or_continue()) — shown once
    // per session, before that menu's own loop with the save-slot picker
    // begins (see main.cpp). The stat/item sidebars are hidden for this and
    // show_intro(), and only revealed once real gameplay begins —
    // set_sidebars_visible() does that.
    void show_title_card();

    // The New Game/Continue menu itself (Continue omitted when `has_save`
    // is false) — returns 0 for New Game or 1 for Continue. Split out from
    // show_title_card() so main.cpp can redisplay just this menu, without
    // the "press A" splash, if the player backs out of the save-slot picker
    // with B.
    int choose_new_or_continue(bool has_save);

    // A centered 3-slot picker, reusing the title menu's background/layout —
    // called right after choose_new_or_continue() returns, for whichever of
    // New Game/Continue was picked. Each slot's label says whether it
    // already holds a save, and if so, which room it would resume in (via
    // SaveGame::peek_room()/room_short_name()), so slots are distinguishable
    // at a glance. `only_occupied` restricts the list to slots that
    // actually have one (Continue; guaranteed at least one exists, since
    // choose_new_or_continue() only offers Continue when
    // SaveGame::any_save_exists() is true) — omitted entirely, any slot may
    // be picked (New Game; picking an occupied one overwrites it, same as
    // the source's slot selection). Returns the chosen slot index (0-based),
    // or -1 if the player backed out with B (see main.cpp's loop back to
    // choose_new_or_continue() in that case).
    int choose_save_slot(bool only_occupied);

    // A centered Music Volume/SFX Volume/Back menu (see engine/settings.h):
    // left/right adjusts whichever row is selected, applying live so
    // adjusting SFX volume is heard immediately; B or picking Back exits,
    // persisting via Settings::save(). Reachable both from
    // choose_new_or_continue()'s own "Settings" row and from the in-game
    // pause menu (see run(), Start), so it's a real screen method rather
    // than folded into either caller.
    void show_settings_menu();

    // The coach-ride opening, shown once on a fresh New Game before the
    // player's first real choice in ForestCrossing. Cut down hard from the
    // source project's cinematic intro scene (color-shifting sky,
    // gamepad prompts) to the story beats themselves, but keeps its
    // layered scrolling tree silhouettes (see assets/rooms/intro_trees_*)
    // as a real parallax effect over the castle art.
    void show_intro();

    // Swaps the current room's background for `item` (e.g. a special
    // full-screen picture — see ChildrensRoom's drawing/GreatChamber's
    // journal-style "look at X" beats), shows `text`, then restores the
    // current room's own background — same create_bg(0, -10) placement
    // every room background already uses, so a "special image" is really
    // just a background asset like any room's, swapped in and back out.
    void show_special_image(const bn::regular_bg_item& item, const bn::string_view& text);

    // Re-applies the current room's background — for a room whose
    // Room::background_override() can change mid-visit (see DrawingRoom's
    // no-raven candles art), called right after whatever choice just made
    // that happen, so the swap is immediate instead of waiting for the next
    // room re-entry.
    void refresh_background();

    void set_sidebars_visible(bool visible);

    // Runs the game forever, starting from `start`.
    [[noreturn]] void run(RoomId start);

    // Shows `text` in the text box and blocks until the player pages through
    // and dismisses it (A on the last page).
    void show_text(const bn::string_view& text);

    // Shows a choice menu and blocks until the player picks one; returns its
    // index.
    int choose(const bn::ivector<bn::string<40>>& options);

    // Blocks until the check is resolved; see Dice::roll_check.
    Result roll(Attribute attribute, int dc, bool cancelable, const bn::string_view& title);

    // Tumbles the on-screen die until it lands on `value` (blocking); it
    // stays visible until hide_die().
    void tumble_die(int value);
    void hide_die();

    // Shows/hides the enemy sprite overlaid on the room background — used
    // both by Combat::fight and by a room's own on_enter() for ambient
    // pre-fight content (the raven already circling in ForestCrossing, the
    // rat already scurrying in KitchenCellar) — matches the source
    // project's per-enemy SpriteAnimations.*() overlay, including its
    // per-instance scale/movement animation (see EnemyAnim) for the enemies
    // that have one.
    void show_enemy(EnemyId id);
    void hide_enemy();

    // Same overlay slot, generalized for a dialogue NPC's portrait
    // (Dialogue::talk) — combat and dialogue never overlap, so one shared
    // sprite slot covers both without needing two. `scale` enlarges small
    // source art (see MadGnome's portrait).
    void show_overlay_sprite(const bn::sprite_item& item, bn::fixed scale = 1);
    void hide_overlay_sprite();

    // Switches the current room: shows `transition_text` first if non-empty,
    // then the new room's background + description (room.gd's add_path).
    void goto_room(RoomId next, const bn::string_view& transition_text = bn::string_view());

    // Permanently ends the game with a win message (Phase 4f's escape
    // endgame) — mirrors _check_game_over's shape: shows the text and
    // never dismisses it, since there's no post-game menu/title screen
    // yet to return to (Phase 4g).
    [[noreturn]] void win();

    // Phase 4h's inventory browser: D-pad left/right cycles owned items,
    // showing each one's name + tooltip description, with a "Use" choice
    // when item_is_usable() allows it. Reached via the global "Inv" entry
    // run() appends to every room's choice list; blocks until the player
    // backs out (B, or choosing "Back").
    void _open_inventory();

    // For widgets that need to draw their own text outside the normal
    // text-box/choice-list flow (e.g. SlidingPuzzle's cell cursor) —
    // sharing this instance instead of constructing another one avoids
    // duplicating the font's setup for no benefit.
    bn::sprite_text_generator& text_generator()
    {
        return _text_generator;
    }

private:
    void _enter_room(RoomId id);
    void _refresh_sidebars();
    void _refresh_ambient(RoomId id);

    // background_item(id), unless that room's own background_override()
    // says otherwise (see DrawingRoom) — the one place that decides which
    // background a room actually gets, used by both _enter_room() and
    // show_special_image()'s restore step so neither has to know about
    // overrides itself.
    const bn::regular_bg_item& _room_background(RoomId id) const;

    // Advances whichever overlay animation is active (see EnemyAnim) by one
    // frame and repositions/re-flips the sprite accordingly — called from
    // every blocking loop that already ticks bn::core::update() (show_text,
    // choose), so ambient wandering keeps moving while the player reads or
    // picks a choice, not just during Combat::fight.
    void _update_enemy_animation();

    // Advances every room's ambient torch/candle flame by one frame — same
    // call sites as _update_enemy_animation(), same reasoning.
    void _update_ambient();

    // Checked at the top of show_text/choose — the two places any story
    // beat or menu passes through — so death from any source (a trap, a
    // fight) is caught in one place instead of every damage call site
    // needing to check it. No death screen/restart exists yet (that needs
    // Phase 3's save system to reset room/enemy state cleanly), so this
    // just halts here permanently once it fires.
    void _check_game_over();

    // The in-game pause menu (Start, checked inside choose() — see run()):
    // Resume/Settings/Main Menu, reusing _choose_centered over whatever
    // room art and choice list are already on screen (both sit in the
    // bottom band, well below this menu's centered rows, so nothing is
    // obscured). "Main Menu" calls bn::core::reset() ([[noreturn]]) rather
    // than trying to unwind back out of however deep the call stack is
    // (Combat::fight, Dialogue::talk, ...) — SRAM (save slots, settings)
    // survives a soft reset, only the in-progress session's state is lost,
    // which is exactly what returning to the title screen should do anyway.
    void _show_pause_menu();

    // Centered New Game/Continue menu for show_title_screen() — a plain
    // vertical list drawn straight through _text_generator (D-pad up/down,
    // A confirms), rather than the bottom-band ChoiceList every in-game
    // choice uses, since a title screen menu wants to sit in the middle of
    // the screen over the art, not in the reserved bottom panel.
    // `cancelable` also accepts B, returning -1 (see choose_save_slot()).
    int _choose_centered(const bn::ivector<bn::string<40>>& options, bool cancelable = false);

    // Overlays `text` centered at screen y `y`, with no panel/text-box
    // backdrop, and blocks until A is pressed — for a prompt drawn directly
    // over full-screen art (the title card), where the bottom text-box
    // band would clip the image instead.
    void _wait_for_a(const bn::string_view& text, bn::fixed y);

    bn::sprite_text_generator _text_generator;
    ui::Panel _panel;
    ui::TextBox _text_box;
    ui::ChoiceList _choices;
    ui::ItemBar _item_bar;
    ui::SidePanel _stats_panel;
    ui::DiceView _dice_view;
    bn::optional<bn::regular_bg_ptr> _background;
    bn::optional<bn::sprite_ptr> _enemy_sprite;

    // Overlay animation state (see EnemyAnim/_update_enemy_animation) —
    // only meaningful while _enemy_sprite is set and _enemy_anim != NONE.
    // RAT_FLOOR reuses _anim_index as the current waypoint target and
    // _anim_paused/_anim_frame for the pause-between-dashes timer; FIGURE_EIGHT
    // reuses _anim_frame as the path angle in degrees (it just counts up
    // forever, wrapping via degrees_sin's own period).
    EnemyAnim _enemy_anim = EnemyAnim::NONE;
    int _anim_frame = 0;
    int _anim_index = 0;
    bool _anim_paused = false;
    bn::fixed _anim_x;

    // Ambient torch/candle overlays for the current room (see AmbientDecor)
    // — one sprite_ptr per decoration, plus its own frame/wait counters
    // since torches (8 frames) and candles (6 frames) cycle at different
    // lengths but the same speed (12 ticks/frame, matching the source's
    // "speed 5.0" SpriteFrames animations at 60 FPS).
    struct AmbientSprite
    {
        bn::sprite_ptr sprite;
        AmbientKind kind;
        int frame = 0;
        int wait = 0;
    };

    static constexpr int max_ambient = 6;
    bn::vector<AmbientSprite, max_ambient> _ambient;

    RoomId _current_room = RoomId::OutsideGates;
    bool _game_over = false;

    // Previous value of each stat shown in _stats_panel (HP, SAN, LCK, INT,
    // STR, DEX, DC, in that order), so _refresh_sidebars can flash a line
    // green/red when it changes. -1 means "not drawn yet" (no flash on the
    // very first draw).
    int _prev_stats[7] = { -1, -1, -1, -1, -1, -1, -1 };

    // Phase 4f's escape pursuit counter: not saved, matching the source's
    // own escape_pursuit (reloading mid-escape just means the pursuers
    // lost the player's trail). Incremented by any non-movement action
    // while is_escaping(); reset by moving to a new room. See
    // _handle_escape_pursuit.
    int _escape_pursuit = 0;
    void _handle_escape_pursuit(bool moved);

    // Phase 4g's title screen/intro hide both sidebars until real gameplay
    // starts; _refresh_sidebars() re-applies this every time it rebuilds
    // their sprites (a fresh sprite_ptr always starts out visible, so
    // without this the very next show_text()/choose() call after hiding
    // them would silently undo it).
    bool _sidebars_visible = true;
};

}

#endif
