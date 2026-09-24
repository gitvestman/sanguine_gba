#ifndef SG_ROOMS_GARDEN_H
#define SG_ROOMS_GARDEN_H

#include "../engine/room.h"
#include "../dialogue/mad_gnome.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\garden.gd, including the
// Mad Gnome encounter — initially cut for scope in the first pass of this
// room (flagged as heavy, ~196 lines, in IMPLEMENTATION_PLAN.md's content
// triage note) and restored per user request after playtesting, same as
// Chapel's puzzle/Ghostly Bride were.
namespace sg::rooms
{

class Garden : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _searched = false;
    dialogue::MadGnome _gnome;
};

}

#endif
