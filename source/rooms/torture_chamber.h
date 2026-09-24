#ifndef SG_ROOMS_TORTURE_CHAMBER_H
#define SG_ROOMS_TORTURE_CHAMBER_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\torture_chamber.gd.
namespace sg::rooms
{

class TortureChamber : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _trap_sprung = false;
};

}

#endif
