#include "panel.h"

#include "bn_sprite_items_panel_tile.h"

namespace sg::ui
{

namespace
{
    // 64x32 tiles (the smallest GBA sprite size at 64px width) centered at
    // y=64 so the strip spans screen y:[48,80) — the bottom 32px only. x
    // centers cover -128..128, slightly wider than the 240px-wide screen
    // (-120..120) so there is no gap at either edge.
    constexpr int bottom_panel_y = 64;
    constexpr int panel_xs[] = { -96, -32, 32, 96 };
}

Panel::Panel()
{
    for(int x : panel_xs)
    {
        _tiles.push_back(bn::sprite_items::panel_tile.create_sprite(x, bottom_panel_y));
    }
}

}
