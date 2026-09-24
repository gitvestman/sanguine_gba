#ifndef SG_ROOMS_CHAPEL_H
#define SG_ROOMS_CHAPEL_H

#include "../engine/room.h"
#include "../ui/sliding_puzzle.h"
#include "../dialogue/ghostly_bride.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\chapel.gd, including the
// gate puzzle (as a D-pad sliding-tile puzzle — see ui::SlidingPuzzle) and
// the Ghostly Bride encounter, both initially cut for scope in the first
// pass of this room and restored per user request after playtesting.
namespace sg::rooms
{

class Chapel : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    // Called by Ossuary's "Up" and HallOfArmors' "N" choices right before
    // goto_room() — same cross-room-write pattern as
    // HallOfArmors::enter_from. Chapel's iron gate blocks the direct route
    // down until it's solved; that's only bypassed while the player is
    // currently coming back up from Ossuary the way they came, not once
    // they've wandered off south and returned, so this is set/cleared on
    // arrival rather than a one-time flag.
    void set_entered_from_ossuary(bool entered_from_ossuary)
    {
        _entered_from_ossuary = entered_from_ossuary;
    }

private:
    bool _searched = false;
    bool _gate_found = false;
    bool _gate_unlocked = false;
    bool _entered_from_ossuary = false;
    ui::SlidingPuzzle _puzzle;
    dialogue::GhostlyBride _bride;
};

}

#endif
