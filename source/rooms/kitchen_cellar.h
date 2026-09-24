#ifndef SG_ROOMS_KITCHEN_CELLAR_H
#define SG_ROOMS_KITCHEN_CELLAR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\kitchen_cellar.gd. A rat
// turns up the first time the player returns here from WineCellar or
// Larder (never on the very first visit), and keeps offering a fight until
// killed — see on_enter.
namespace sg::rooms
{

class KitchenCellar : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    // Called by WineCellar/Larder's own "go back" choices right before
    // goto_room() — the same cross-room-write pattern HallOfArmors uses for
    // enter_from. Only takes effect on a return visit, never the first.
    void announce_rat_if_visited()
    {
        if(visited)
        {
            _rat_appeared = true;
        }
    }

private:
    bool _rat_active() const;

    bool _rat_appeared = false;
};

}

#endif
