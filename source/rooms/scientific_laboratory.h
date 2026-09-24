#ifndef SG_ROOMS_SCIENTIFIC_LABORATORY_H
#define SG_ROOMS_SCIENTIFIC_LABORATORY_H

#include "../engine/room.h"
#include "../dialogue/golem.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\scientific_laboratory.gd.
// The electric-spark visual effect and its looping sound cue are cut (no
// gameplay effect); the lightning trap (DEX check) and the Golem encounter
// are real.
namespace sg::rooms
{

class ScientificLaboratory : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _trap_sprung = false;
    bool _golem_approached = false;
    bool _found_cog = false;
    dialogue::Golem _golem;
};

}

#endif
