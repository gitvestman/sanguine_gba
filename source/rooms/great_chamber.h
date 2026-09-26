#ifndef SG_ROOMS_GREAT_CHAMBER_H
#define SG_ROOMS_GREAT_CHAMBER_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\great_chamber.gd. The
// Milly-specific journal framing/sanity bonus is cut (no such character in
// this build); the journal's own reveal-a-paragraph-at-a-time structure
// (each one granting the item it describes) is kept as-is.
namespace sg::rooms
{

class GreatChamber : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _found_journal = false;
    bool _read_journal = false;
    int _journal_reveal_step = 0;
};

}

#endif
