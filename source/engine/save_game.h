#ifndef SG_SAVE_GAME_H
#define SG_SAVE_GAME_H

#include "room.h"

// SRAM save/load — 3 slots (see IMPLEMENTATION_PLAN.md's Phase 3), matching
// the source project's MAX_SAVE_SLOTS; each slot is its own fixed-offset
// SaveData record (see save_game.cpp), so they never overlap. Mirrors
// GameState.autosave() in gamestate_singleton.gd: saved after every room
// change, so a mid-fight death never overwrites the last checkpoint (combat
// never changes rooms while it's in progress — see Combat::fight).
namespace sg
{

class SaveGame
{

public:
    static constexpr int slot_count = 3;

    // True if a save from this build's version is present in `slot`.
    static bool save_exists(int slot);

    // True if any slot has one — the title screen uses this to decide
    // whether "Continue" is even offered.
    static bool any_save_exists();

    // The room a save in `slot` would resume in, without touching
    // GameState/Inventory/room state at all — for the save-slot picker's
    // "Slot N - <room>" labels, which must describe a slot without
    // disturbing whatever game (if any) is currently running. Returns
    // OutsideGates for an empty slot, same as load_game()'s own fallback.
    static RoomId peek_room(int slot);

    // Restores GameState/Inventory/every room's state from `slot` and
    // returns the room to resume in. Also marks `slot` as the active slot
    // for subsequent save() calls, same as start_new_game() below.
    static RoomId load_game(int slot);

    // Marks `slot` as the active slot for a fresh game — everything else
    // stays at its just-constructed default (nothing is read from SRAM);
    // subsequent save() calls write here.
    static void start_new_game(int slot);

    // Writes GameState/Inventory/every room's state, plus `current_room`,
    // to the active slot set by load_game()/start_new_game().
    static void save(RoomId current_room);
};

}

#endif
