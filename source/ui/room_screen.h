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

    // Phase 4g's title screen: shows the game's name and a New Game/
    // Continue choice (Continue omitted when `has_save` is false), and
    // returns 0 for New Game or 1 for Continue. Reuses the same text box/
    // choice list every other screen in the game uses rather than a
    // separate title-screen widget — the stat/item sidebars are visible
    // behind it too (harmless before a save is loaded, if not perfectly
    // polished — a real title screen without them is future work).
    int show_title_screen(bool has_save);

    // The coach-ride opening, shown once on a fresh New Game before the
    // player's first real choice in ForestCrossing. Cut down hard from the
    // source project's cinematic intro scene (color-shifting sky, sway/
    // particle effects, gamepad prompts) to the story beats themselves.
    void show_intro();

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

    // Shows/hides the enemy sprite overlaid on the room background during
    // combat (Combat::fight) — matches the source project's per-enemy
    // SpriteAnimations.*() overlay.
    void show_enemy(EnemyId id);
    void hide_enemy();

    // Same overlay slot, generalized for a dialogue NPC's portrait
    // (Dialogue::talk) — combat and dialogue never overlap, so one shared
    // sprite slot covers both without needing two.
    void show_overlay_sprite(const bn::sprite_item& item);
    void hide_overlay_sprite();

    // Switches the current room: shows `transition_text` first if non-empty,
    // then the new room's background + description (room.gd's add_path).
    void goto_room(RoomId next, const bn::string_view& transition_text = bn::string_view());

    // Permanently ends the game with a win message (Phase 4f's escape
    // endgame) — mirrors _check_game_over's shape: shows the text and
    // never dismisses it, since there's no post-game menu/title screen
    // yet to return to (Phase 4g).
    [[noreturn]] void win();

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

    // Checked at the top of show_text/choose — the two places any story
    // beat or menu passes through — so death from any source (a trap, a
    // fight) is caught in one place instead of every damage call site
    // needing to check it. No death screen/restart exists yet (that needs
    // Phase 3's save system to reset room/enemy state cleanly), so this
    // just halts here permanently once it fires.
    void _check_game_over();

    bn::sprite_text_generator _text_generator;
    ui::Panel _panel;
    ui::TextBox _text_box;
    ui::ChoiceList _choices;
    ui::ItemBar _item_bar;
    ui::SidePanel _stats_panel;
    ui::DiceView _dice_view;
    bn::optional<bn::regular_bg_ptr> _background;
    bn::optional<bn::sprite_ptr> _enemy_sprite;
    RoomId _current_room = RoomId::ForestCrossing;
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
};

}

#endif
