#ifndef SG_ROOMS_TROPHY_ROOM_H
#define SG_ROOMS_TROPHY_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\trophy_room.gd. The
// achievement/diary-entry calls are cut (neither system exists in this
// build); the actual mechanic (search the mounted antlers, but only after
// AudienceHall's hidden switch has been triggered) is kept as-is.
namespace sg::rooms
{

class TrophyRoom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _search_attempted = false;
    bool _tried_antlers = false;
    bool _found_secret_door = false;
};

}

#endif
