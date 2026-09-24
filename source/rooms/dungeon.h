#ifndef SG_ROOMS_DUNGEON_H
#define SG_ROOMS_DUNGEON_H

#include "../engine/room.h"
#include "../dialogue/prisoner.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\dungeon.gd. The wooden-door
// branch talks to the Prisoner dialogue (see dialogue/prisoner.h). The
// "chase them out" branch's fight is real (Combat::fight).
namespace sg::rooms
{

enum class DungeonEncounter
{
    INTRO,
    SEARCH,
    FREE_CREATURES,
    CHASE_OUT
};

class Dungeon : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    DungeonEncounter _encounter = DungeonEncounter::INTRO;
    bool _dungeon_search = false;
    bool _creatures_resolved = false;
    bool _wooden_door_opened = false;
    dialogue::Prisoner _prisoner;
};

}

#endif
