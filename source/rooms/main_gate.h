#ifndef SG_ROOMS_MAIN_GATE_H
#define SG_ROOMS_MAIN_GATE_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\main_gate.gd. The open/
// closed background swap (tied to Courtyard's portcullis) is cut — one
// static image, matching DinnerHall's own clean/dirty image already being
// simplified to one background.
namespace sg::rooms
{

class MainGate : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
