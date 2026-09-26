#ifndef SG_ROOMS_BEDCHAMBER_01_H
#define SG_ROOMS_BEDCHAMBER_01_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\bedchamber_01.gd and its
// paired dialogue/zombie.gd. The source keeps the zombie's "conversation"
// as a separate Dialogue class that reads the room's own encounter_state
// back out of it — since it's this tightly coupled to one room's state
// already (not reusable elsewhere the way Prisoner/GhostlyBride are),
// it's folded directly into this room's own state machine instead of
// going through the generic Dialogue engine.
namespace sg::rooms
{

enum class ZombieEncounter
{
    START,
    ANNOUNCED,
    HORROR_SUCCESS,
    HORROR_FAIL,
    STILL,
    EMBRACED
};

class BedChamber01 : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void on_enter(RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _passage_found = false;
    bool _peaceful = false;
    ZombieEncounter _encounter = ZombieEncounter::START;
};

}

#endif
