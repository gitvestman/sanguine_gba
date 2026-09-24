#ifndef SG_ROOMS_GUEST_BEDROOM_H
#define SG_ROOMS_GUEST_BEDROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\guest_bedroom.gd. The
// Countess-hiding-here branch (set by DinnerHall's "dine with her" choice
// in the source project — cut from our DinnerHall in Phase 4b, and the
// Countess herself isn't ported until Phase 4e) is left out for the same
// reason as FancyCorridor01's Countess intro.
namespace sg::rooms
{

class GuestBedroom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
