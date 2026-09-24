#ifndef SG_SAVE_GAME_H
#define SG_SAVE_GAME_H

#include "room.h"

// SRAM save/load — one slot (see IMPLEMENTATION_PLAN.md's Phase 3; a
// multi-slot title screen is deferred until there's a second floor to make
// slot selection meaningful). Mirrors GameState.autosave() in
// gamestate_singleton.gd: saved after every room change, so a mid-fight
// death never overwrites the last checkpoint (combat never changes rooms
// while it's in progress — see Combat::fight).
namespace sg
{

class SaveGame
{

public:
    // Restores GameState/Inventory/every room's state from SRAM and
    // returns the room to resume in, if a save from this build's version is
    // present; otherwise leaves everything at its just-constructed default
    // and returns CellarHall (a fresh game).
    static RoomId load_or_new_game();

    // Writes GameState/Inventory/every room's state, plus `current_room`,
    // to SRAM.
    static void save(RoomId current_room);
};

}

#endif
