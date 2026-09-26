#ifndef SG_ROOMS_STAIRWAY_1STFLOOR_H
#define SG_ROOMS_STAIRWAY_1STFLOOR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stairway_1stfloor.gd. Both
// "go up" (Stairway2ndFloor) and "go down" (StairwayBasement) are real.
namespace sg::rooms
{

class Stairway1stFloor : public Room
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
};

}

#endif
