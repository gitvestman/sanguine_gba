#ifndef SG_DIALOGUE_COUNTESS_H
#define SG_DIALOGUE_COUNTESS_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\countess.gd — trimmed
// hard, same as every other NPC dialogue in this build. The dinner
// invitation is real (restored after user request — see DinnerHall's own
// handling of dinner_pending()), but simplified: no per-character unique
// dinner conversation, and declining doesn't set up GuestBedroom's hiding/
// later-ambush branch (that needs a whole extra enemy-hiding system this
// build doesn't have) — declining just lets her go, same as attacking her
// outright.
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

    // True from the moment the player accepts her dinner invitation until
    // DinnerHall resolves the scene (dine peacefully, or fight her there
    // instead) — DinnerHall reads this to know a dinner is owed.
    bool dinner_pending() const
    {
        return _dinner_pending;
    }

    void set_dinner_pending(bool pending)
    {
        _dinner_pending = pending;
    }

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;

private:
    bool _resolved = false;
    bool _dinner_pending = false;
};

}

#endif
