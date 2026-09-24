#ifndef SG_ROOMS_AUDIENCE_HALL_H
#define SG_ROOMS_AUDIENCE_HALL_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\audience_hall.gd.
namespace sg::rooms
{

class AudienceHall : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    // Readable cross-room by TrophyRoom, whose own secret door depends on
    // this switch — matching CellarHall::north_secret_door_found.
    bool switch_triggered() const
    {
        return _switch_triggered;
    }

private:
    bool _found_hole = false;
    bool _finger_attempted = false;
    bool _switch_triggered = false;
};

}

#endif
