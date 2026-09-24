#ifndef SG_DIALOGUE_COUNTESS_H
#define SG_DIALOGUE_COUNTESS_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\countess.gd — trimmed
// hard, same as every other NPC dialogue in this build, but this one loses
// more structure than most: the source's dinner-invitation branch feeds
// DinnerHall's Countess dinner scene and GuestBedroom's hiding branch,
// both of which were already cut in Phase 4b (they need a whole extra
// scene/enemy-hiding system). So here, declining to fight just lets her
// pass — no dinner, no hiding, no later ambush — matching what those two
// rooms actually still support.
namespace sg::dialogue
{

class Countess : public Dialogue
{

public:
    // True once she's been let go peacefully — FancyCorridor01 uses this
    // (alongside the Countess enemy's own is_dead()) to stop re-triggering
    // the encounter.
    bool resolved() const
    {
        return _resolved;
    }

    void set_resolved(bool resolved)
    {
        _resolved = resolved;
    }

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;

private:
    bool _resolved = false;
};

}

#endif
