#ifndef SG_INVENTORY_H
#define SG_INVENTORY_H

#include "bn_vector.h"

// Minimal stand-in for GameState's item table (gamestate_singleton.gd's
// `items`/ItemId/add_item/has_item). Only the items the Basement floor's
// locked doors and skill-check rewards actually reference are listed;
// amounts/ammo/tooltips/UI (the rest of Godot's item dicts) are Phase 3's
// full inventory system, not this vertical slice's concern — this just
// needs to answer "do we have the key" truthfully.
namespace sg
{

enum class ItemId
{
    IRON_KEY,
    SKULL_KEY,
    LOCKPICKS,
    SILVER_DAGGER,
    FLINTLOCK,
    RESTORATIVE_MIXTURE,
    HEALING_TINCTURE,
    HOMUNCULUS,
    COG,
    JOURNAL,
    STRANGE_FIGURINE,
    DOLL,
    BASTET_FIGURINE,
    SHEET_MUSIC,
    TOY_SOLDIER,
    MEZUZAH_AMULET,

    COUNT
};

class Inventory
{

public:
    static Inventory& instance();

    bool has(ItemId item) const
    {
        return _owned[int(item)];
    }

    // The order items were actually picked up in, oldest first — the item
    // sidebar and the inventory browser both read this (reversed, so the
    // most recently found item shows first/topmost) instead of walking
    // ItemId in its fixed enum order, so a player can tell what they just
    // found at a glance instead of it jumping to wherever that ItemId
    // happens to sit in the list.
    const bn::ivector<ItemId>& discovery_order() const
    {
        return _discovery_order;
    }

    void add(ItemId item)
    {
        if(! _owned[int(item)])
        {
            _owned[int(item)] = true;
            _discovery_order.push_back(item);
        }
    }

    void remove(ItemId item)
    {
        if(_owned[int(item)])
        {
            _owned[int(item)] = false;

            for(int i = 0; i < _discovery_order.size(); ++i)
            {
                if(_discovery_order[i] == item)
                {
                    _discovery_order.erase(_discovery_order.begin() + i);
                    break;
                }
            }
        }
    }

    // The flintlock is this build's one item with a per-shot resource
    // (gamestate_singleton.gd's fire_flintlock()/item.ammo) rather than a
    // simple owned/not-owned flag — everything else here answers "do we
    // have it", this answers "how many shots are left". Starts at 3,
    // matching CHARACTER_SELECT_FLINTLOCK_DESCRIPTION's "three shots".
    int flintlock_ammo() const
    {
        return _flintlock_ammo;
    }

    // Fires one shot if any remain (decrementing and returning true) or
    // reports an empty gun (false) without changing anything — mirrors
    // fire_flintlock()'s own ammo>0 check.
    bool fire_flintlock()
    {
        if(_flintlock_ammo <= 0)
        {
            return false;
        }

        --_flintlock_ammo;
        return true;
    }

    // For SaveGame's round-trip only.
    void set_flintlock_ammo(int ammo)
    {
        _flintlock_ammo = ammo;
    }

private:
    Inventory() = default;

    bool _owned[int(ItemId::COUNT)] = {};
    bn::vector<ItemId, int(ItemId::COUNT)> _discovery_order;
    int _flintlock_ammo = 3;
};

}

#endif
