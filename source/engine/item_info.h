#ifndef SG_ITEM_INFO_H
#define SG_ITEM_INFO_H

#include "bn_string_view.h"
#include "bn_sprite_item.h"

#include "inventory.h"

// Phase 4h's usable inventory: a name + short tooltip description per item
// (the source project has no dedicated per-item tooltip text to port — its
// only per-item strings are the bare display names used in choice labels —
// so these are written fresh, matching this port's own terse style) plus
// whether it can be consumed from RoomScreen's inventory browser.
namespace sg
{

class RoomScreen;

struct ItemInfo
{
    bn::string_view name;
    bn::string_view description;
};

const ItemInfo& item_info(ItemId item);

// The 32x32 icon sprite item for this item — shared by ItemBar's sidebar
// icons and the inventory browser, so the lookup is defined once.
const bn::sprite_item& item_icon(ItemId item);

// True for the handful of consumables (currently just the two potions) —
// everything else (keys, curios, quest items) is browsable but has nothing
// to "use" outside the one puzzle/door it already unlocks.
bool item_is_usable(ItemId item);

// Applies the item's effect, removes it from the inventory, and shows the
// resulting flavor text — shared by Combat::fight's mid-fight item use and
// the inventory browser's own "Use" choice, so the effect is defined once.
void use_item(ItemId item, RoomScreen& screen);

}

#endif
