#ifndef SG_ROOMS_STAIRWAY_2NDFLOOR_H
#define SG_ROOMS_STAIRWAY_2NDFLOOR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stairway_2ndfloor.gd — the
// source has no "go up" choice here either (only Down and North); the Third
// Floor connects via FancyCorridor01's own north exit to FancyCorridor02,
// not through this stairway. An early "Up" stub speculating otherwise was
// left in this room past Phase 4e actually wiring up the real Third Floor
// entrance, letting the player try to go somewhere that was never coming —
// removed once noticed in play.
namespace sg::rooms
{

class Stairway2ndFloor : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _searched = false;
    bool _figurine_found = false;
    bool _figurine_taken = false;
};

}

#endif
