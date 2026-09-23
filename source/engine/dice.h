#ifndef SG_DICE_H
#define SG_DICE_H

#include "bn_string_view.h"

#include "constants.h"

// Ported from D:\Workspace\Godot\sanguine\scenes\dice_board.gd's roll
// resolution (_on_show_dice / _roll_result): d20 + attribute bonus vs a
// difficulty-adjusted DC, natural 1 always critical-fails, natural 20 always
// grants +1 Luck when it also succeeds, and a plain fail can be reroll'd for
// 1 Luck while any Luck remains. The 3D physics die is replaced by a 2D
// sprite tumble (ui::DiceView, via RoomScreen::tumble_die).
namespace sg
{

class RoomScreen;

class Dice
{

public:
    // Blocks until the check is resolved. `dc` is the room's raw DC (before
    // difficulty adjustment, matching add_skill_check_choice's `dc`
    // parameter in room.gd).
    static Result roll_check(RoomScreen& screen, Attribute attribute, int dc, bool cancelable,
                              const bn::string_view& title);
};

}

#endif
