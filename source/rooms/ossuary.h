#ifndef SG_ROOMS_OSSUARY_H
#define SG_ROOMS_OSSUARY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\ossuary.gd.
namespace sg::rooms
{

class Ossuary : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;

private:
    bool _search_skeleton_key = false;
};

}

#endif
