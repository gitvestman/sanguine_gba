#ifndef SG_ROOMS_GREAT_HALL_H
#define SG_ROOMS_GREAT_HALL_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\great_hall.gd — the First
// Floor's hub room. Music (hall_music) is dropped: Phase 5 is where audio
// gets scoped and added, not folded silently into a content phase.
namespace sg::rooms
{

class GreatHall : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
