#ifndef SG_ROOM_H
#define SG_ROOM_H

#include <cstdint>

#include "bn_string.h"
#include "bn_string_view.h"
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

    // First Floor (Phase 4b)
    GreatHall,
    Courtyard,
    Kitchen,
    DinnerHall,
    HallOfArmors,
    MusicRoom,
    Chapel,
    Storage,
    KitchenCellar,
    WineCellar,
    Orangery,
    Larder,
    Stairway1stFloor,

    // Outside (Phase 4c)
    ForestCrossing,
    GroundskeeperCottage,
    Garden,
    OvergrownPond,
    MainGate,
    KitchenEntrance,
    Cemetery,
    Mausoleum,
    Tombstones,
    CliffTombs,

    // Second Floor (Phase 4d)
    FancyCorridor01,
    BedChamber01,
    GreatChamber,
    Library,
    HallOfPortraits,
    AudienceHall,
    Treasury,
    Bathroom,
    ChildrensRoom,
    Stairway2ndFloor,
    TrophyRoom,
    GuestBedroom,

    // Third Floor (Phase 4e)
    FancyCorridor02,
    Reliquary,
    Observatory,
    Skybridge,
    ToymakersWorkshop,
    DrawingRoom,
    ScientificLaboratory,

    // Escape endgame (Phase 4f)
    OutsideGates,

    COUNT
};

struct Choice
{
    bn::string<40> text;
    int action_id;
};

// Phase 4i follow-up: the source project's Room.corridor_torches()/
// doorway_torches()/candles() overlay small looping flame-flicker animations
// (graphics/rooms/Torches_*.tres, Candle_Frames.tres) on top of the static
// room art — unconditional ambient set dressing, not tied to room state, so
// a fixed per-room list built once on room entry is enough (see
// RoomScreen::show_enemy's per-frame animator for something that DOES need
// to react to state/movement; this is simpler, just a looping frame cycle).
// Positions are given in RoomScreen's centered sprite coordinates, i.e.
// already converted from the source's 180x140 room-canvas coordinates via
// (source_x - 90, source_y - 80) — see convert_room_bg.py's centering math.
enum class AmbientKind
{
    TORCH,
    CANDLE
};

struct AmbientDecor
{
    AmbientKind kind;
    bn::fixed x;
    bn::fixed y;
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

    // Appends this room's ambient torches/candles (see AmbientDecor) — most
    // rooms have none, hence the no-op default.
    virtual void build_ambient(bn::ivector<AmbientDecor>& /*out*/) const {}

    // Lets a room swap its own background based on internal state instead
    // of the fixed per-RoomId mapping background_item() returns — e.g.
    // DrawingRoom's candles-with-no-bird art once the Night Raven is gone
    // (get_image()/image_noraven in the source). nullptr (the default)
    // means "use background_item(id) as normal".
    virtual const bn::regular_bg_item* background_override() const
    {
        return nullptr;
    }

    // Packs/restores this room's whole persistent state (visited plus every
    // subclass-specific flag) into/from a single byte for SaveGame — every
    // room in this floor fits its own state in a handful of bits. Bit 0 is
    // always `visited`; a subclass override must chain to the base for that
    // bit and use bits 1+ for its own fields (see e.g. Dungeon, whose
    // 4-value _encounter enum needs 2 of them).
    virtual uint16_t save_state() const
    {
        return visited ? 1 : 0;
    }

    virtual void load_state(uint16_t bits)
    {
        visited = bits & 1;
    }

    bool visited = false;
};

Room& room(RoomId id);

// The Butano regular_bg_item generated for this room's converted background
// (see tools/convert_room_bg.py and assets/rooms/*.bmp+json).
const bn::regular_bg_item& background_item(RoomId id);

// A short, map-label-style name for this room (e.g. "the source project's
// own <TR_PREFIX>_MAP" localization strings) — used by the save-slot picker
// to say where each slot's save actually is, not gameplay text.
bn::string_view room_short_name(RoomId id);

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
