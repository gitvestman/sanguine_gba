#ifndef SG_ROOMS_STONE_CORRIDOR_H
#define SG_ROOMS_STONE_CORRIDOR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\stone_corridor.gd. The
// zodiac button_puzzle.gd minigame behind the north door is simplified to
// an INT skill check with the same effect (unlocks the door) — a dedicated
// button-sequence minigame is its own scoped feature, not part of the
// room-graph/dice vertical slice.
namespace sg::rooms
{

class StoneCorridor : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    bool puzzle_door_open = false;

private:
    bool _examined_puzzle_door = false;
};

}

#endif
