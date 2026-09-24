#ifndef SG_ROOMS_CELLAR_HALL_H
#define SG_ROOMS_CELLAR_HALL_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\cellar_hall.gd. The
// south exit to CliffTombs (an OUTSIDE-level room) is omitted: it's outside
// this vertical slice's Basement floor.
namespace sg::rooms
{

class CellarHall : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

    // Readable cross-room by StoneCorridor, matching Godot's
    // current_room_state.get_room_state("CELLAR_HALL", "north_secret_door_found", ...).
    bool north_secret_door_found = false;

private:
    bool _trap_sprung = false;
};

// Typed accessor for cross-room state reads (StoneCorridor checks/sets
// north_secret_door_found from its own side of the same secret door).
CellarHall& cellar_hall();

}

#endif
