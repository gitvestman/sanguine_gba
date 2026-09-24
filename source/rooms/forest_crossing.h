#ifndef SG_ROOMS_FOREST_CROSSING_H
#define SG_ROOMS_FOREST_CROSSING_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\forest_crossing.gd. The
// south exit to OutsideGates (not one of this sub-phase's 10 rooms) is
// stubbed. The raven's passive "pecks you every few turns" timer is cut —
// it lived inside get_choices(), which our build_choices() has no
// RoomScreen access from (can't show text or deal damage there); a real
// "Attack raven" fight covers the actual mechanic without it.
namespace sg::rooms
{

class ForestCrossing : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _found_path = false;
    bool _search_attempted = false;
};

}

#endif
