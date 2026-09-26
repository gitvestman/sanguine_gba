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
    int _flintlock_ammo = 3;
};

}

#endif
