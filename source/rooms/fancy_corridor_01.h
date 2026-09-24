#ifndef SG_ROOMS_FANCY_CORRIDOR_01_H
#define SG_ROOMS_FANCY_CORRIDOR_01_H

#include "../engine/room.h"
#include "../dialogue/countess.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\fancy_corridor_01.gd — the
// Second Floor's 7-door hub, and the Countess's first appearance. Her
// dinner-invitation branch (which feeds DinnerHall's Countess dinner scene
// and GuestBedroom's hiding branch, both cut in Phase 4b) is trimmed away:
// declining to fight her here just lets her go, with no later ambush.
namespace sg::rooms
{

class FancyCorridor01 : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void on_enter(RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _countess_met = false;
    dialogue::Countess _countess;
};

}

#endif
