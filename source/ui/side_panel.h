#ifndef SG_UI_SIDE_PANEL_H
#define SG_UI_SIDE_PANEL_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

// A narrow (32px) vertical strip along the left or right screen edge over a
// solid backdrop, showing a short list of text lines — used for the
// character-stats sidebar (right) and the owned-items sidebar (left). Spans
// screen y:[-80,48) (the full height above TextBox/ChoiceList's bottom
// panel band), built from two stacked 32x64 sprite tiles. The room art's
// own 180x140 painting already leaves ~30px of border margin on each side
// once centered on a 240px-wide screen, so this mostly covers padding
// rather than real art.
namespace sg::ui
{

class SidePanel
{

public:
    static constexpr int max_lines = 12;

    // x is this panel's horizontal center (negative for the left edge,
    // positive for the right edge).
    SidePanel(bn::sprite_text_generator& generator, bn::fixed x);

    void set_lines(const bn::ivector<bn::string<8>>& lines);

private:
    bn::sprite_text_generator& _generator;
    bn::fixed _x;
    bn::vector<bn::sprite_ptr, 2> _background;
    bn::vector<bn::sprite_ptr, max_lines * 4> _text_sprites;
};

}

#endif
