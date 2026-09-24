#ifndef SG_ROOMS_LARDER_H
#define SG_ROOMS_LARDER_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\larder.gd. The source
// room's fly-swarm decoration is pure Godot node/tween art with no
// gameplay effect, so it's dropped rather than reimplemented as sprites.
namespace sg::rooms
{

class Larder : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint8_t save_state() const override;
    void load_state(uint8_t bits) override;

private:
    bool _searched = false;
    bool _barrels_opened = false;
    bool _steeled = false;
};

}

#endif
