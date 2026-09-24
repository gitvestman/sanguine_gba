#ifndef SG_ROOMS_WINE_CELLAR_H
#define SG_ROOMS_WINE_CELLAR_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\wine_cellar.gd.
namespace sg::rooms
{

class WineCellar : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _wine_tasted = false;
};

}

#endif
