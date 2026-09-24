#ifndef SG_ROOMS_LIBRARY_H
#define SG_ROOMS_LIBRARY_H

#include "../engine/room.h"

// Ported from D:\Workspace\Godot\sanguine\rooms\library.gd.
namespace sg::rooms
{

class Library : public Room
{

public:
    void describe(bn::string<768>& out, bool force_long = false) override;
    void build_choices(bn::ivector<Choice>& out) override;
    void on_choice(int action_id, RoomScreen& screen) override;
    uint16_t save_state() const override;
    void load_state(uint16_t bits) override;

private:
    bool _search_attempted = false;
    bool _book_found = false;
    bool _passage_found = false;
    bool _figurine_found = false;
};

}

#endif
