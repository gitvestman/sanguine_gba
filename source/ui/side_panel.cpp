#include "side_panel.h"

#include "bn_algorithm.h"
#include "bn_sprite_items_stats_panel_tile.h"

namespace sg::ui
{

namespace
{
    // Two 32x64 tiles stacked cover y:[-80,48) exactly (2*64=128 == 48-(-80)).
    constexpr bn::fixed top_tile_y = -48;
    constexpr bn::fixed bottom_tile_y = 16;

    constexpr bn::fixed text_top_y = -78;
    constexpr bn::fixed line_height = 8;
    constexpr bn::fixed half_glyph_height = 4;

    // Left-inset from the panel's own center so text starts a couple of
    // pixels in from the panel's left edge (panel is 32px wide).
    constexpr bn::fixed text_left_inset = 14;
}

SidePanel::SidePanel(bn::sprite_text_generator& generator, bn::fixed x) :
    _generator(generator),
    _x(x)
{
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, top_tile_y));
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, bottom_tile_y));
}

void SidePanel::set_lines(const bn::ivector<bn::string<8>>& lines)
{
    _text_sprites.clear();
    _generator.set_left_alignment();

    bn::fixed text_x = _x - text_left_inset;
    int count = bn::min(lines.size(), max_lines);

    for(int i = 0; i < count; ++i)
    {
        bn::fixed y = text_top_y + (line_height * i) + half_glyph_height;
        _generator.generate(text_x, y, lines[i], _text_sprites);
    }
}

}
