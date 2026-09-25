#ifndef SG_ROOMS_OUTSIDE_GATES_H
#define SG_ROOMS_OUTSIDE_GATES_H

#include "../engine/room.h"
#include "../engine/enemy.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\outside_gates.gd — the
// very first room (the coach-ride intro ends here, same as the source
// project) and, once the Countess is dead, the escape endgame's final
// room. The source's multi-step first-visit description (two "Continue"
// presses to reveal the full text) and its "look at the castle" scroll
// effect are collapsed into one plain description, per user request —
// the scroll doesn't exist in this port anyway (no continuous background
// scrolling outside the intro), so there was nothing left to page through.
namespace sg::rooms
{

class OutsideGates : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void on_enter(RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _guard_assigned = false;
    EnemyId _guard = EnemyId::COUNT;
};

}

#endif
