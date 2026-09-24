#ifndef SG_ROOMS_COURTYARD_H
#define SG_ROOMS_COURTYARD_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\courtyard.gd — cut down
// hard. The source room has a whole multi-state saga here (climbing down
// the well, a rope-grab check, a south gate you can crawl under or raise
// the portcullis for) that all exists to eventually reach MainGate/Garden —
// Outside-level rooms that are Phase 4c's job, not this one's. Until that
// floor exists the south gate is a dead end, so it's a single stub line
// (matching the convention StairwayBasement/Dungeon already use for
// not-yet-ported links) instead of building state machinery for a path
// that goes nowhere yet. The well is kept as one simple STR check, since
// it's a self-contained reward and doesn't need Garden to make sense.
namespace sg::rooms
{

class Courtyard : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    bool _searched = false;
    bool _well_inspected = false;
};

}

#endif
