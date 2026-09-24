#ifndef SG_ROOMS_TREASURY_H
#define SG_ROOMS_TREASURY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\treasury.gd.
namespace sg::rooms
{

class Treasury : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _searched = false;
    bool _chest_open = false;
};

}

#endif
