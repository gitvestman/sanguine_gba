#ifndef SG_ROOMS_ARMORY_ROOM_H
#define SG_ROOMS_ARMORY_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\armory_room.gd. The
// character-specific flavor line (Mehmet recognizing the heraldry) is kept,
// since GameState::instance() already carries a character stat block we can
// key off of once character selection exists — for now it just always shows
// the generic version (character_id isn't tracked yet in this slice).
namespace sg::rooms
{

class ArmoryRoom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;

private:
    bool _trap_sprung = false;
};

}

#endif
