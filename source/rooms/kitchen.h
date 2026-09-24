#ifndef SG_ROOMS_KITCHEN_H
#define SG_ROOMS_KITCHEN_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\kitchen.gd. The source room
// has a hidden "south doors" reveal leading to KitchenEntrance (an
// Outside-level room, Phase 4c's job) — only the Courtyard half of that
// reveal is kept, since Courtyard already exists in this floor.
namespace sg::rooms
{

class Kitchen : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _meat_taken = false;
    bool _south_wall_inspected = false;
};

}

#endif
