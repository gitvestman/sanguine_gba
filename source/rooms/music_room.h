#ifndef SG_ROOMS_MUSIC_ROOM_H
#define SG_ROOMS_MUSIC_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\music_room.gd. Playing the
// Sheet Music (granted by GreatChamber, 2nd floor) grants a random item,
// matching the source's GameState.add_random_item() — this was originally
// cut as unreachable (Sheet Music didn't exist anywhere in the build yet),
// but 4d added the item via GreatChamber without ever wiring this payoff
// back up.
namespace sg::rooms
{

class MusicRoom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _organ_played = false;
    bool _sheet_music_played = false;
};

}

#endif
