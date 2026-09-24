#ifndef SG_ROOMS_STAIRWAY_BASEMENT_H
#define SG_ROOMS_STAIRWAY_BASEMENT_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stairway_basement.gd.
namespace sg::rooms
{

class StairwayBasement : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
