#ifndef SG_UI_ITEM_BAR_H
#define SG_UI_ITEM_BAR_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"

#include "../engine/inventory.h"

// The left sidebar: owned items as 32x32 icons (converted from the source
// project's graphics/items/*.png by tools/convert_item_icon.py), stacked
// top to bottom over the same dark backdrop as the stats sidebar. The
// sidebar is 128px tall, so it holds 4 icons — enough for every item
// obtainable on the Basement floor; more floors will need a smaller icon
// size or paging.
namespace sg::ui
{

class ItemBar
{

public:
    static constexpr int max_icons = 4;

    explicit ItemBar(bn::fixed x);

    void set_items(const bn::ivector<ItemId>& items);

private:
    bn::fixed _x;
    bn::vector<bn::sprite_ptr, 2> _background;
    bn::vector<bn::sprite_ptr, max_icons> _icons;
};

}

#endif
