#ifndef SG_INVENTORY_H
#define SG_INVENTORY_H

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
    RESTORATIVE_MIXTURE,
    HEALING_TINCTURE,
    HOMUNCULUS,
    COG,

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

    void add(ItemId item)
    {
        _owned[int(item)] = true;
    }

    void remove(ItemId item)
    {
        _owned[int(item)] = false;
    }

private:
    Inventory() = default;

    bool _owned[int(ItemId::COUNT)] = {};
};

}

#endif
