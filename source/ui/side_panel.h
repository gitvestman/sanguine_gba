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

// A per-line hint for set_lines: recolor that line green/red for one redraw,
// to flag a stat that just went up or down.
enum class StatFlash
{
    NONE,
    GAIN,
    LOSS
};

class SidePanel
{

public:
    static constexpr int max_lines = 12;

    // x is this panel's horizontal center (negative for the left edge,
    // positive for the right edge).
    SidePanel(bn::sprite_text_generator& generator, bn::fixed x);

    // codes and values must be the same size (one entry per stat line);
    // flashes may be shorter (or empty) — missing entries are treated as
    // StatFlash::NONE. Each line renders as `codes[i]` left-aligned and
    // `values[i]` right-aligned, so the numbers form a straight column
    // regardless of the code's width.
    void set_lines(const bn::ivector<bn::string<4>>& codes, const bn::ivector<bn::string<4>>& values,
                    const bn::ivector<StatFlash>& flashes);

private:
    bn::sprite_text_generator& _generator;
    bn::fixed _x;
    bn::vector<bn::sprite_ptr, 2> _background;
    bn::sprite_ptr _portrait;
    bn::vector<bn::sprite_ptr, max_lines * 4> _text_sprites;
};

}

#endif
