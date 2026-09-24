#ifndef SG_ROOMS_OBSERVATORY_H
#define SG_ROOMS_OBSERVATORY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\observatory.gd. The
// zodiac-chart popup (a whole separate scene/diary-image in the source
// project) is cut — the choice stays, just as a plain flavor line.
namespace sg::rooms
{

class Observatory : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
