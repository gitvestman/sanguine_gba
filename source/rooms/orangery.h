#ifndef SG_ROOMS_ORANGERY_H
#define SG_ROOMS_ORANGERY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\orangery.gd — cut down
// hard. The source room's whole point is a DEX-gated crossing past
// malevolent plants to reach Garden, an Outside-level room that's Phase
// 4c's job; until that floor exists there's nothing south to walk toward,
// so the plants/tread mechanic is dropped rather than built for a
// destination that doesn't exist yet (same reasoning as Courtyard's south
// gate). The room still exists as a one-way spur off GreatHall.
namespace sg::rooms
{

class Orangery : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    bool _searched = false;
};

}

#endif
