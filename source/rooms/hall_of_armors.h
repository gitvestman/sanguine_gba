#ifndef SG_ROOMS_HALL_OF_ARMORS_H
#define SG_ROOMS_HALL_OF_ARMORS_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\hall_of_armors.gd. The
// Silver Rosary shortcut is cut (no such item exists in this build yet, and
// it would be an unreachable dead branch otherwise, same reasoning as the
// vertical slice's other floor already applied to weapon items).
//
// The source tracks "which door did the player enter from" via a
// last_room_class the whole game state exposes; this port has no such
// global and doesn't need one just for this — Chapel's and GreatHall's own
// "go to Hall of Armors" choices call enter_from() directly before
// goto_room(), the same cross-room-write pattern StoneCorridor/CellarHall
// already use for their shared secret door.
namespace sg::rooms
{

class HallOfArmors : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

    // 0 = unknown, 1 = north (from Chapel), 2 = south (from GreatHall).
    void enter_from(int side)
    {
        _entered_from = side;
    }

private:
    void _maybe_trigger_trap(int exit_side, RoomScreen& screen);

    bool _trap_found = false;
    bool _trap_sprung = false;
    bool _searched = false;
    int _entered_from = 0;
};

}

#endif
