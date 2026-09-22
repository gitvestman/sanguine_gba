#ifndef SG_DICE_H
#define SG_DICE_H

#include "bn_string_view.h"

#include "constants.h"
#include "../ui/text_box.h"
#include "../ui/choice_list.h"

// Ported from D:\Workspace\Godot\sanguine\scenes\dice_board.gd's roll
// resolution (_on_show_dice / _roll_result): d20 + attribute bonus vs a
// difficulty-adjusted DC, natural 1 always critical-fails, natural 20 always
// grants +1 Luck when it also succeeds, and a plain fail can be reroll'd for
// 1 Luck while any Luck remains. The physical 3D die is replaced by this
// text readout for now (see IMPLEMENTATION_PLAN.md's Dice roll section) —
// no die-face sprite art exists yet, and the roll math/flow is what this
// vertical slice needs to prove; a later phase can swap in the animated
// sprite without touching this logic.
namespace sg
{

class Dice
{

public:
    // Blocks (running its own bn::core::update() loop) until the check is
    // resolved. `dc` is the room's raw DC (before difficulty adjustment,
    // matching add_skill_check_choice's `dc` parameter in room.gd).
    static Result roll_check(ui::TextBox& text_box, ui::ChoiceList& choices, Attribute attribute, int dc,
                              bool cancelable, const bn::string_view& title);
};

}

#endif
