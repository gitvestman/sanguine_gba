#ifndef SG_ROOMS_MUSIC_ROOM_H
#define SG_ROOMS_MUSIC_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\music_room.gd. The Sheet
// Music item/reward branch is cut — no such item exists in this build
// (nothing on this floor grants it), so it would be permanently unreachable
// dead code rather than a real choice.
namespace sg::rooms
{

class MusicRoom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    bool _organ_played = false;
};

}

#endif
