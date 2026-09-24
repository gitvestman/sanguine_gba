#ifndef SG_ROOMS_CHILDRENS_ROOM_H
#define SG_ROOMS_CHILDRENS_ROOM_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\childrens_room.gd. The
// folding-painting popup (a whole separate scene/animation in the source
// project) is cut — the drawing's sanity-loss beat is kept as a plain
// show_text instead. The hidden stairs to ToymakersWorkshop are real.
namespace sg::rooms
{

class ChildrensRoom : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _drawing_found = false;
    bool _drawing_seen = false;
    bool _room_search_attempted = false;
    bool _found_stairs = false;
    bool _doll_search_attempted = false;
};

}

#endif
