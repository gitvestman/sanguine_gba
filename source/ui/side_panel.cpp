#include "side_panel.h"

#include "bn_algorithm.h"
#include "bn_array.h"
#include "bn_sprite_items_stats_panel_tile.h"
#include "bn_sprite_items_portrait_dario.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_palette_ptr.h"
#include "common_variable_8x8_sprite_font.h"

namespace sg::ui
{

namespace
{
    // The font's own palette is a shaded, anti-aliased ramp (not flat white),
    // so a flash can't just swap one color — every shade needs re-tinting
    // into the same green/red hue, keeping each color's brightness.
    bn::array<bn::color, 16> tint_palette(bool green)
    {
        const bn::span<const bn::color>& source =
            common::variable_8x8_sprite_font.item().palette_item().colors_ref();
        bn::array<bn::color, 16> result;

        for(int i = 0; i < 16; ++i)
        {
            bn::color c = source[i];
            int brightness = bn::max(bn::max(c.red(), c.green()), c.blue());
            result[i] = green ? bn::color(0, brightness, 0) : bn::color(brightness, 0, 0);
        }

        return result;
    }

    bn::sprite_palette_ptr gain_palette()
    {
        static bn::array<bn::color, 16> colors = tint_palette(true);
        static bn::sprite_palette_ptr palette =
            bn::sprite_palette_item(colors, bn::bpp_mode::BPP_4).create_palette();
        return palette;
    }

    bn::sprite_palette_ptr loss_palette()
    {
        static bn::array<bn::color, 16> colors = tint_palette(false);
        static bn::sprite_palette_ptr palette =
            bn::sprite_palette_item(colors, bn::bpp_mode::BPP_4).create_palette();
        return palette;
    }

    // Two 32x64 tiles stacked cover y:[-80,48) exactly (2*64=128 == 48-(-80)).
    constexpr bn::fixed top_tile_y = -48;
    constexpr bn::fixed bottom_tile_y = 16;

    // The 32x64 portrait sits over the top tile, so stat text starts below
    // it (y:[-16,48), i.e. the bottom half of the panel) instead of at the
    // panel's very top.
    constexpr bn::fixed portrait_y = -48;
    constexpr bn::fixed text_top_y = -14;
    constexpr bn::fixed line_height = 8;
    constexpr bn::fixed half_glyph_height = 4;

    // Left-inset from the panel's own center so the code starts a couple of
    // pixels in from the panel's left edge, and right-inset so the number
    // ends a couple of pixels in from the right edge (panel is 32px wide).
    constexpr bn::fixed text_left_inset = 14;
    constexpr bn::fixed text_right_inset = 0;
}

SidePanel::SidePanel(bn::sprite_text_generator& generator, bn::fixed x) :
    _generator(generator),
    _x(x),
    _portrait(bn::sprite_items::portrait_dario.create_sprite(x, portrait_y))
{
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, top_tile_y));
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, bottom_tile_y));

    // Portrait must render above the backdrop tiles behind it.
    _portrait.set_z_order(-1);
}

void SidePanel::set_lines(const bn::ivector<bn::string<4>>& codes, const bn::ivector<bn::string<4>>& values,
                           const bn::ivector<StatFlash>& flashes)
{
    _text_sprites.clear();

    bn::fixed code_x = _x - text_left_inset;
    bn::fixed value_x = _x + (32 / 2) - text_right_inset;
    int count = bn::min(codes.size(), max_lines);

    for(int i = 0; i < count; ++i)
    {
        bn::fixed y = text_top_y + (line_height * i) + half_glyph_height;
        int sprites_before = _text_sprites.size();

        _generator.set_left_alignment();
        _generator.generate(code_x, y, codes[i], _text_sprites);

        _generator.set_right_alignment();
        _generator.generate(value_x, y, values[i], _text_sprites);

        StatFlash flash = i < flashes.size() ? flashes[i] : StatFlash::NONE;

        if(flash != StatFlash::NONE)
        {
            bn::sprite_palette_ptr palette = flash == StatFlash::GAIN ? gain_palette() : loss_palette();

            for(int s = sprites_before; s < _text_sprites.size(); ++s)
            {
                _text_sprites[s].set_palette(palette);
            }
        }
    }
}

}
