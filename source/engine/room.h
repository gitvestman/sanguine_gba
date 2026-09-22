#ifndef SG_ROOM_H
#define SG_ROOM_H

#include "bn_string.h"
#include "bn_vector.h"
#include "bn_regular_bg_item.h"

#include "inventory.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\room.gd. Godot's Room stores
// its choices as {id, text, action-closure, key} and GameState.choose() calls
// the closure directly for ordinary room choices (id/key only matter for the
// combat/dialogue paths, which this slice doesn't have yet). C++ has no
// no-allocation equivalent of a captured closure, so each Choice instead
// carries a small `action_id` that the room's own on_choice() switches on —
// same "one room, one set of choices, one dispatch" shape, different
// mechanism, matching how the plan already treats GlobalMessenger's dispatch
// pattern.
namespace sg
{

class RoomScreen;

enum class RoomId
{
    CellarHall,
    Ossuary,
    Dungeon,
    TortureChamber,
    StoneCorridor,
    ArmoryRoom,
    AlchemicalLaboratory,
    StairwayBasement,
    TheMachine,
    HomunculusWorkshop,

    COUNT
};

struct Choice
{
    bn::string<40> text;
    int action_id;
};

class Room
{

public:
    virtual ~Room() = default;

    // Appends this room's current description (long on first visit, short
    // after — mirrors get_long_short_description) to `out`, flipping
    // `visited` in the process just like Godot's set_state("room_visited").
    // force_long re-shows the long description regardless of `visited`
    // (the "look around" choice's effect in room.gd).
    virtual void describe(bn::string<768>& out, bool force_long = false) = 0;

    virtual void build_choices(bn::ivector<Choice>& out) = 0;

    virtual void on_choice(int action_id, RoomScreen& screen) = 0;

    // Called right after the background/description are shown for a room
    // that was just entered (room.gd's enter_room() override hook).
    virtual void on_enter(RoomScreen& /*screen*/) {}

    bool visited = false;
};

Room& room(RoomId id);

// The Butano regular_bg_item generated for this room's converted background
// (see tools/convert_room_bg.py and assets/rooms/*.bmp+json).
const bn::regular_bg_item& background_item(RoomId id);

// Whether the door gated by `key` is currently passable: owning `key`
// itself, or the Skull Key (a master key for any IronKey-gated door in
// Godot's add_locked_path). Skips add_locked_path's Lockpicks/Silver Dagger
// pick-lock sub-branches: no room in this floor grants either item, so
// they're unreachable in this slice regardless.
inline bool item_unlocks_path(ItemId key)
{
    return Inventory::instance().has(key) || Inventory::instance().has(ItemId::SKULL_KEY);
}

}

#endif
