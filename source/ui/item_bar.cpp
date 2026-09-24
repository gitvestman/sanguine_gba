#include "item_bar.h"

#include "bn_sprite_item.h"
#include "bn_sprite_items_stats_panel_tile.h"
#include "bn_sprite_items_item_iron_key.h"
#include "bn_sprite_items_item_skull_key.h"
#include "bn_sprite_items_item_lockpicks.h"
#include "bn_sprite_items_item_silver_dagger.h"
#include "bn_sprite_items_item_restorative_mixture.h"
#include "bn_sprite_items_item_healing_tincture.h"
#include "bn_sprite_items_item_homunculus.h"
#include "bn_sprite_items_item_cog.h"
#include "bn_sprite_items_item_journal.h"
#include "bn_sprite_items_item_strange_figurine.h"
#include "bn_sprite_items_item_doll.h"
#include "bn_sprite_items_item_bastet_figurine.h"
#include "bn_sprite_items_item_toy_soldier.h"

namespace sg::ui
{

namespace
{
    // Same backdrop layout as SidePanel: two 32x64 tiles covering y:[-80,48).
    constexpr bn::fixed top_tile_y = -48;
    constexpr bn::fixed bottom_tile_y = 16;

    // Centers of the four 32px icon slots within y:[-80,48).
    constexpr bn::fixed icon_ys[ItemBar::max_icons] = { -64, -32, 0, 32 };

    const bn::sprite_item& icon_for(ItemId item)
    {
        switch(item)
        {

        case ItemId::IRON_KEY:
            return bn::sprite_items::item_iron_key;

        case ItemId::SKULL_KEY:
            return bn::sprite_items::item_skull_key;

        case ItemId::LOCKPICKS:
            return bn::sprite_items::item_lockpicks;

        case ItemId::SILVER_DAGGER:
            return bn::sprite_items::item_silver_dagger;

        case ItemId::RESTORATIVE_MIXTURE:
            return bn::sprite_items::item_restorative_mixture;

        case ItemId::HEALING_TINCTURE:
            return bn::sprite_items::item_healing_tincture;

        case ItemId::HOMUNCULUS:
            return bn::sprite_items::item_homunculus;

        case ItemId::COG:
            return bn::sprite_items::item_cog;

        case ItemId::JOURNAL:
            return bn::sprite_items::item_journal;

        case ItemId::STRANGE_FIGURINE:
            return bn::sprite_items::item_strange_figurine;

        case ItemId::DOLL:
            return bn::sprite_items::item_doll;

        case ItemId::BASTET_FIGURINE:
            return bn::sprite_items::item_bastet_figurine;

        case ItemId::TOY_SOLDIER:
            return bn::sprite_items::item_toy_soldier;

        // Sheet Music has no dedicated icon in the source project either;
        // falls back to the Cog's icon rather than adding a placeholder.
        case ItemId::SHEET_MUSIC:
        default:
            return bn::sprite_items::item_cog;
        }
    }
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
        _icons.push_back(icon_for(items[i]).create_sprite(_x, icon_ys[i]));
        _icons.back().set_z_order(-1);
    }
}

}
