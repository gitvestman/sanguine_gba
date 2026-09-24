#include "childrens_room.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A small, dust-choked nursery, toys scattered across the floor as if abandoned "
        "mid-play.";
    constexpr bn::string_view short_description = "The children's room.";
}

void ChildrensRoom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void ChildrensRoom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "Look at bookshelf", 2 });

    if(_drawing_found && ! _drawing_seen)
    {
        out.push_back({ "Look at drawing", 3 });
    }

    if(! _room_search_attempted)
    {
        out.push_back({ "Search", 4 });
    }

    if(_found_stairs)
    {
        out.push_back({ "Ascend stairs", 5 });
    }

    if(! _doll_search_attempted)
    {
        out.push_back({ "Search for doll", 6 });
    }
}

void ChildrensRoom::on_choice(int action_id, RoomScreen& screen)
{
    switch(action_id)
    {

    case 0:
    {
        bn::string<768> text;
        describe(text, true);
        screen.show_text(text);
        break;
    }

    case 1:
        screen.goto_room(RoomId::FancyCorridor01);
        break;

    case 2:
        _drawing_found = true;
        screen.show_text("Among faded picture books, a child's drawing lies folded between "
                          "the pages — a woman with too many teeth, smiling.");
        break;

    case 3:
        _drawing_seen = true;
        GameState::instance().change_sanity(-1);
        screen.show_text("The longer you look, the more the smile seems to move. You lose 1 "
                          "Sanity.");
        break;

    case 4:
    {
        _room_search_attempted = true;
        Result result = screen.roll(Attribute::INT_, 10, true, "Search");

        if(result == Result::SUCCESS)
        {
            _found_stairs = true;
            screen.show_text("Behind a false panel in the closet, a narrow stair spirals "
                              "upward.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("Just toys and dust.");
        }

        break;
    }

    case 5:
        screen.goto_room(RoomId::ToymakersWorkshop);
        break;

    case 6:
    {
        _doll_search_attempted = true;
        Result result = screen.roll(Attribute::INT_, 12, false, "Search for doll");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::DOLL);
            screen.show_text("Under the bed, you find a porcelain doll, its painted face "
                              "eerily lifelike.");
        }
        else
        {
            screen.show_text("You find nothing under the bed.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t ChildrensRoom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_drawing_found) bits |= 1 << 1;
    if(_drawing_seen) bits |= 1 << 2;
    if(_room_search_attempted) bits |= 1 << 3;
    if(_found_stairs) bits |= 1 << 4;
    if(_doll_search_attempted) bits |= 1 << 5;
    return bits;
}

void ChildrensRoom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _drawing_found = bits & (1 << 1);
    _drawing_seen = bits & (1 << 2);
    _room_search_attempted = bits & (1 << 3);
    _found_stairs = bits & (1 << 4);
    _doll_search_attempted = bits & (1 << 5);
}

}
