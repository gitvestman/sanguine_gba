#ifndef SG_ROOMS_ALCHEMICAL_LABORATORY_H
#define SG_ROOMS_ALCHEMICAL_LABORATORY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\alchemical_laboratory.gd.
namespace sg::rooms
{

class AlchemicalLaboratory : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;

private:
    bool _progress_forward = false;
    bool _found_potion = false;
};

}

#endif
