#ifndef SG_ROOMS_STAIRWAY_1STFLOOR_H
#define SG_ROOMS_STAIRWAY_1STFLOOR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stairway_1stfloor.gd. The
// "go up" exit to Stairway2ndFloor is stubbed: the Second Floor isn't part
// of the port yet (Phase 4d). "Go down" to StairwayBasement is real —
// completing the link that room's own header already reserved.
namespace sg::rooms
{

class Stairway1stFloor : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    bool _searched = false;
};

}

#endif
