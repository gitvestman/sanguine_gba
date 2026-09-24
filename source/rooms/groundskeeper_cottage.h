#ifndef SG_ROOMS_GROUNDSKEEPER_COTTAGE_H
#define SG_ROOMS_GROUNDSKEEPER_COTTAGE_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\groundskeepers_cottage.gd.
// The Groundskeeper NPC has no branching conversation in the source
// project either (just one flavor line dispatched on "talk"), so this
// stays a plain repeatable show_text rather than a full Dialogue subclass.
namespace sg::rooms
{

class GroundskeeperCottage : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
