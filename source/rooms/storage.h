#ifndef SG_ROOMS_STORAGE_H
#define SG_ROOMS_STORAGE_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\storage.gd. The Strange
// Figurine / hidden vault branch is cut — that item doesn't exist in this
// build, so the vault it unlocks would be permanently unreachable.
namespace sg::rooms
{

class Storage : public Room
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
