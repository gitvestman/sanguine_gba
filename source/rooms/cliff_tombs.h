#ifndef SG_ROOMS_CLIFF_TOMBS_H
#define SG_ROOMS_CLIFF_TOMBS_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\clifftombs.gd. The
// successful-climb ending (crawling in through a window into Bathroom)
// is real. A fatal fall is real too: it drops HP to 0, same as any other
// source of lethal damage.
namespace sg::rooms
{

class CliffTombs : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    // Set by CellarHall's own secret-door-south choice right before
    // goto_room() — matches Godot's set_state("found_hidden", true) on
    // that same path.
    void set_found_hidden()
    {
        _found_hidden = true;
    }

private:
    bool _found_hidden = false;
    bool _search_attempted = false;
    bool _climb_resolved = false;
    bool _high_on_cliff = false;
};

}

#endif
