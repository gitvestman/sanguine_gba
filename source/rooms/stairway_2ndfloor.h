#ifndef SG_ROOMS_STAIRWAY_2NDFLOOR_H
#define SG_ROOMS_STAIRWAY_2NDFLOOR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stairway_2ndfloor.gd. The
// "go up" exit to Stairway3rdFloor... wait, actually to whatever the
// Third Floor's own stairway is, is stubbed: Phase 4e's job.
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
