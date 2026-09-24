#ifndef SG_ROOMS_TOMBSTONES_H
#define SG_ROOMS_TOMBSTONES_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\tombstones.gd. The Doll
// item reward branch is cut — no such item exists in this build (nothing
// grants it), so it would be a permanently dead choice. The diary-entry
// system referenced by "Inspect" doesn't exist yet either; that choice is
// kept as a plain flavor line instead.
namespace sg::rooms
{

class Tombstones : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _searched = false;
};

}

#endif
