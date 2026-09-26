#include "item_bar.h"

#include "bn_sprite_item.h"
#include "bn_sprite_items_stats_panel_tile.h"

#include "../engine/item_info.h"

namespace sg::ui
{

namespace
{
    // Same backdrop layout as SidePanel: two 32x64 tiles covering y:[-80,48).
    constexpr bn::fixed top_tile_y = -48;
    constexpr bn::fixed bottom_tile_y = 16;

    // Centers of the four 32px icon slots within y:[-80,48).
    constexpr bn::fixed icon_ys[ItemBar::max_icons] = { -64, -32, 0, 32 };
}

ItemBar::ItemBar(bn::fixed x) :
    _x(x)
{
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, top_tile_y));
    _background.push_back(bn::sprite_items::stats_panel_tile.create_sprite(x, bottom_tile_y));
}

void ItemBar::set_items(const bn::ivector<ItemId>& items)
{
    _icons.clear();

    for(int i = 0; i < items.size() && i < max_icons; ++i)
    {
        _icons.push_back(item_icon(items[i]).create_sprite(_x, icon_ys[i]));
        _icons.back().set_z_order(-1);
    }
}

void ItemBar::set_visible(bool visible)
{
    for(bn::sprite_ptr& sprite : _background)
    {
        sprite.set_visible(visible);
    }

    for(bn::sprite_ptr& sprite : _icons)
    {
        sprite.set_visible(visible);
    }
}

}
