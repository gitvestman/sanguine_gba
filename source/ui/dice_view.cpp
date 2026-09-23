#include "dice_view.h"

#include "bn_core.h"
#include "bn_string.h"
#include "bn_sprite_items_die.h"

#include "common_variable_8x16_sprite_font.h"

#include "../engine/rng.h"

namespace sg::ui
{

namespace
{
    // Middle of the room art visible between the top of the screen and the
    // bottom text panel (screen y:[-80,48)), clear of both sidebars.
    constexpr bn::fixed die_x = 0;
    constexpr bn::fixed die_y = -16;
    constexpr bn::fixed hop_height = 3;
    constexpr int frames_count = 3;

    // Frames between face changes: fast at first, slowing to a stop.
    constexpr int step_frames[] = { 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 8, 10 };
    constexpr int landed_hold_frames = 30;
}

DiceView::DiceView() :
    _number_generator(common::variable_8x16_sprite_font)
{
    _number_generator.set_center_alignment();
    _number_generator.set_z_order(-1);
}

void DiceView::tumble_to(int value)
{
    int frame = 0;
    int face = value;
    int step = 0;

    for(int wait : step_frames)
    {
        frame = (frame + 1) % frames_count;

        int next_face = rng().get_int(1, 21);
        face = (next_face == face) ? (next_face % 20) + 1 : next_face;

        bn::fixed y = (step % 2 == 0) ? die_y - hop_height : die_y;
        _show(face, frame, y);
        ++step;

        for(int i = 0; i < wait; ++i)
        {
            bn::core::update();
        }
    }

    _show(value, 0, die_y);

    for(int i = 0; i < landed_hold_frames; ++i)
    {
        bn::core::update();
    }
}

void DiceView::hide()
{
    _die.reset();
    _number.clear();
}

void DiceView::_show(int face, int frame, bn::fixed y)
{
    if(bn::sprite_ptr* die = _die.get())
    {
        die->set_tiles(bn::sprite_items::die.tiles_item(), frame);
        die->set_y(y);
    }
    else
    {
        _die = bn::sprite_items::die.create_sprite(die_x, y, frame);
    }

    _number.clear();
    _number_generator.generate(die_x, y, bn::to_string<4>(face), _number);
}

}
