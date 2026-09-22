#ifndef SG_ROOMS_DUNGEON_H
#define SG_ROOMS_DUNGEON_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\dungeon.gd. The wooden-door
// branch talks to the Prisoner dialogue and the "chase them out" branch
// starts a real fight in Godot; neither dialogue nor combat exist yet
// (Phase 2+), so both are stubbed to a short placeholder line that still
// advances the room's own state machine — see on_choice.
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

private:
    DungeonEncounter _encounter = DungeonEncounter::INTRO;
    bool _dungeon_search = false;
    bool _creatures_resolved = false;
    bool _wooden_door_opened = false;
};

}

#endif
