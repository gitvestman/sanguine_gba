#ifndef SG_ROOMS_DINNER_HALL_H
#define SG_ROOMS_DINNER_HALL_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\dinner_hall.gd. The
// Countess dinner scene is real (restored after user request), but
// simplified: no per-character unique dinner conversation (this build
// hardcodes one character, Dario, so it uses the source's
// DINNER_HALL_TRANSFORMED_DARIO text) — dining with her just resolves the
// encounter peacefully. What's kept from the original cut is the room's
// own trap: an INT check against the dread that fills the hall, exactly
// like every other floor's first-visit hazard.
namespace sg::rooms
{

class DinnerHall : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    void on_enter(RoomScreen& screen) override;
    void build_ambient(bn::ivector<AmbientDecor>& out) const override;
    const bn::regular_bg_item* background_override() const override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _trap_sprung = false;
    bool _searched = false;
};

}

#endif
