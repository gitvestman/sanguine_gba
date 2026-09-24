#ifndef SG_ROOMS_CEMETERY_H
#define SG_ROOMS_CEMETERY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\cemetery.gd — the Myling
// encounter (a restless dead child, resolved through skill checks and a
// final name-guessing beat) is fully self-contained, so it's ported in
// full rather than cut. Once investigation starts, movement choices
// disappear until it resolves (one way or another) — matching the source
// project exactly, not a simplification.
namespace sg::rooms
{

enum class MylingApproach
{
    NONE,
    ANNOUNCED,
    CLOSE_SUCCESS,
    CLOSE_FAIL
};

enum class MylingTriState
{
    NONE,
    SUCCESS,
    FAIL
};

class Cemetery : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    void _add_name_choices(bn::ivector<Choice>& out) const;
    void _resolve_myling();

    bool _myling_resolved = false;
    bool _investigation_started = false;
    bool _searched = false;
    bool _spoke_to_myling = false;
    MylingApproach _approached = MylingApproach::NONE;
    MylingTriState _understood = MylingTriState::NONE;
    MylingTriState _horror_withstood = MylingTriState::NONE;
};

}

#endif
