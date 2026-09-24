#ifndef SG_ROOMS_FANCY_CORRIDOR_01_H
#define SG_ROOMS_FANCY_CORRIDOR_01_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\fancy_corridor_01.gd — the
// Second Floor's 7-door hub. The Countess encounter that normally starts
// here on first entry is deliberately not wired up yet: she's Phase 4e's
// job (the Countess enemy + dialogue don't exist until then), so this
// room is plumbed for it (matching the plan's own note that she's ported
// alongside her own sub-phase, not smuggled into an earlier one) rather
// than stubbed with a placeholder line.
namespace sg::rooms
{

class FancyCorridor01 : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
};

}

#endif
