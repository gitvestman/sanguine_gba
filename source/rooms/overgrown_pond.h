#ifndef SG_ROOMS_OVERGROWN_POND_H
#define SG_ROOMS_OVERGROWN_POND_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\overgrown_pond.gd — a
// linear one-time encounter (inspect -> three stone throws -> something
// answers -> evade or take the consequences), fully portable as-is.
namespace sg::rooms
{

enum class PondEncounter
{
    START,
    INSPECTED,
    STONE_1,
    STONE_2,
    CREATURE,
    RESOLVED
};

class OvergrownPond : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    PondEncounter _encounter = PondEncounter::START;
};

}

#endif
