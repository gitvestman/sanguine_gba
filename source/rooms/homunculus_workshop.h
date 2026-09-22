#ifndef SG_ROOMS_HOMUNCULUS_WORKSHOP_H
#define SG_ROOMS_HOMUNCULUS_WORKSHOP_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\homunculus_workshop.gd. A
// failed creation attempt starts a fight with the Homunculus in Godot;
// combat isn't ported yet (Phase 2), so it's stubbed the same way as
// Dungeon's fight branch — see on_choice.
namespace sg::rooms
{

class HomunculusWorkshop : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;

private:
    bool _progress_forward = false;
    bool _found_artifact = false;
};

}

#endif
