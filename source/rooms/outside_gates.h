#ifndef SG_ROOMS_OUTSIDE_GATES_H
#define SG_ROOMS_OUTSIDE_GATES_H

#include "../engine/room.h"
#include "../engine/enemy.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\outside_gates.gd. The
// pre-escape flavor content (the multi-step first-visit description, the
// "look at the castle" scroll effect) is cut — it never mattered
// mechanically, and the whole point of this room in this build is the
// escape endgame. During a normal (non-escaping) visit this is just a
// plain room; once the Countess is dead, reaching here means a final
// guard (if anything survived) then a win.
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
