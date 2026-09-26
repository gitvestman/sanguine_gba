#ifndef SG_DIALOGUE_COUNTESS_H
#define SG_DIALOGUE_COUNTESS_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\countess.gd — trimmed
// hard, same as every other NPC dialogue in this build. The dinner
// invitation is real (restored after user request — see DinnerHall's own
// handling of dinner_pending()), simplified to no per-character unique
// dinner conversation. Declining the invitation still just lets her go
// (matches DINNER_DECLINE in the source too — the hiding/ambush branch is
// specifically tied to fleeing FancyCorridor01's fight, see below).
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

    // True once she's fled FancyCorridor01's fight (Combat::fight's
    // flee_after_hits) and gone to ground in GuestBedroom — mirrors the
    // source's room_state("GUEST_BEDROOM", "countess_hiding", ...). Like
    // dinner_pending(), FancyCorridor01 treats this as another reason she's
    // no longer lingering there; GuestBedroom uses it to offer "Confront
    // the Countess". Cleared once she's actually dead, not on its own.
    bool hiding_in_guest_bedroom() const
    {
        return _hiding_in_guest_bedroom;
    }

    void set_hiding_in_guest_bedroom(bool hiding)
    {
        _hiding_in_guest_bedroom = hiding;
    }

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;

private:
    bool _resolved = false;
    bool _dinner_pending = false;
    bool _hiding_in_guest_bedroom = false;
};

}

#endif
