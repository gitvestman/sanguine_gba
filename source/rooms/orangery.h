#ifndef SG_ROOMS_ORANGERY_H
#define SG_ROOMS_ORANGERY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\orangery.gd. The source
// room's south exit to Garden is DEX-gated past malevolent plants (an
// Orangery Plants hostile) — cut, matching Chapel/Garden's own cut
// encounters: it's a real fight system for one room's flavor, not worth
// building until there's a reason to think it matters in play (restore it
// the same way as Chapel's puzzle if it turns out to). South is a plain,
// safe path instead.
namespace sg::rooms
{

class Orangery : public Room
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
