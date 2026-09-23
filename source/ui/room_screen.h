#ifndef SG_UI_ROOM_SCREEN_H
#define SG_UI_ROOM_SCREEN_H

#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "panel.h"
#include "text_box.h"
#include "choice_list.h"
#include "side_panel.h"
#include "item_bar.h"
#include "dice_view.h"
#include "../engine/room.h"
#include "../engine/constants.h"

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

    // Switches the current room: shows `transition_text` first if non-empty,
    // then the new room's background + description (room.gd's add_path).
    void goto_room(RoomId next, const bn::string_view& transition_text = bn::string_view());

private:
    void _enter_room(RoomId id);
    void _refresh_sidebars();

    bn::sprite_text_generator _text_generator;
    ui::Panel _panel;
    ui::TextBox _text_box;
    ui::ChoiceList _choices;
    ui::ItemBar _item_bar;
    ui::SidePanel _stats_panel;
    ui::DiceView _dice_view;
    bn::optional<bn::regular_bg_ptr> _background;
    RoomId _current_room = RoomId::CellarHall;
};

}

#endif
