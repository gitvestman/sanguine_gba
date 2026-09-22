#ifndef SG_ROOMS_THE_MACHINE_H
#define SG_ROOMS_THE_MACHINE_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\the_machine.gd. The 3-second
// chant/sound-duck pause while "using" the fixed machine is dropped — no
// audio/timer system yet, and it's not load-bearing for the vertical slice.
// The Cog needed to fix it isn't obtainable anywhere in this floor (matches
// Godot: it's found elsewhere in the manor), so that branch is reachable
// only once later floors are ported.
namespace sg::rooms
{

class TheMachine : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;

private:
    bool _machine_fixed = false;
    bool _machine_used = false;
};

}

#endif
