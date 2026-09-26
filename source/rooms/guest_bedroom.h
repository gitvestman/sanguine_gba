#ifndef SG_ROOMS_GUEST_BEDROOM_H
#define SG_ROOMS_GUEST_BEDROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\guest_bedroom.gd, including
// the Countess-hiding-here branch: fleeing her FancyCorridor01 fight (see
// Combat::fight's flee_after_hits) sends her here instead of resolving the
// encounter — dialogue::Countess::hiding_in_guest_bedroom() is the shared
// flag (owned by FancyCorridor01, same cross-room-read pattern DinnerHall
// already uses for dinner_pending()).
namespace sg::rooms
{

class GuestBedroom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
};

}

#endif
