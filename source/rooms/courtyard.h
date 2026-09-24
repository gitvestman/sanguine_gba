#ifndef SG_ROOMS_COURTYARD_H
#define SG_ROOMS_COURTYARD_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\courtyard.gd. The
// climb-down-the-well/rope-grab branch (reached only after failing the
// rope pull) is still cut — a whole extra encounter state for a path that
// just leads back out the same well, not worth the complexity here. The
// south gate is real now that Phase 4c's MainGate exists: raise the
// portcullis (STR) for a permanent open path, or crawl under it (DEX,
// cancelable) for a one-off risky shortcut each time.
namespace sg::rooms
{

class Courtyard : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

    // Readable cross-room by MainGate (its own north exit mirrors this
    // same portcullis), matching CellarHall::north_secret_door_found.
    bool portcullis_raised = false;

private:
    bool _searched = false;
    bool _well_inspected = false;
};

}

#endif
