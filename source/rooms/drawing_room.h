#ifndef SG_ROOMS_DRAWING_ROOM_H
#define SG_ROOMS_DRAWING_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\drawingroom.gd. The
// Creature-character-specific greeting variant is cut (no such character
// in this build); the "give it a shiny bauble" peaceful resolution is kept
// but doesn't require a specific found item — noticing it's a jay-like
// creature (the Identify check) is enough.
namespace sg::rooms
{

class DrawingRoom : public Room
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
    bool _raven_gone = false;
    bool _searched = false;
    bool _greeted = false;
    bool _scare_failed = false;
    bool _identify_attempted = false;
    bool _identify_success = false;
    bool _scare_attempted = false;
};

}

#endif
