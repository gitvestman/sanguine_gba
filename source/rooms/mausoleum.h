#ifndef SG_ROOMS_MAUSOLEUM_H
#define SG_ROOMS_MAUSOLEUM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\mausoleum.gd.
namespace sg::rooms
{

class Mausoleum : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _inspected = false;
    bool _push_success = false;
    bool _item_found = false;
    bool _item_taken = false;
};

}

#endif
