#ifndef SG_ROOMS_DINNER_HALL_H
#define SG_ROOMS_DINNER_HALL_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\dinner_hall.gd. The
// Countess dinner-scene branch is cut entirely for this pass — it needs a
// dedicated hostile enemy, a per-character dialogue class, and a
// GuestBedroom cross-room flag that don't exist yet, and belongs with
// whichever later sub-phase actually introduces the Countess (Phase 4e),
// not bundled quietly into a floor she doesn't otherwise appear on. What's
// kept is the room's own trap: an INT check against the dread that fills
// the hall, exactly like every other floor's first-visit hazard.
namespace sg::rooms
{

class DinnerHall : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _trap_sprung = false;
    bool _searched = false;
};

}

#endif
