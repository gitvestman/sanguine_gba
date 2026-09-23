#ifndef SG_UI_DICE_VIEW_H
#define SG_UI_DICE_VIEW_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

// The on-screen d20 for skill checks (IMPLEMENTATION_PLAN.md's "Dice roll"
// section): a sprite in the middle of the room art that tumbles — rotating
// frames, flickering random faces, slowing down — then lands on the rolled
// value. Purely presentation; the roll itself is decided by Dice.
namespace sg::ui
{

class DiceView
{

public:
    DiceView();

    // Blocks (runs its own bn::core::update() loop) until the die has
    // tumbled and landed on `value`. The die stays visible afterwards.
    void tumble_to(int value);

    void hide();

private:
    void _show(int face, int frame, bn::fixed y);

    bn::sprite_text_generator _number_generator;
    bn::optional<bn::sprite_ptr> _die;
    bn::vector<bn::sprite_ptr, 4> _number;
};

}

#endif
