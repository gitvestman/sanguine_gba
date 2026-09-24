#include "library.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Towering bookshelves crowd this dusty library, candlelight flickering over cracked "
        "spines.";
    constexpr bn::string_view short_description = "The library.";
}

void Library::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Library::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "S", 1 });

    if(_book_found)
    {
        if(_passage_found)
        {
            out.push_back({ "N (secret passage)", 2 });
        }
        else
        {
            out.push_back({ "Take the book", 3 });
        }

        if(! _figurine_found)
        {
            out.push_back({ "Search for figurine", 4 });
        }
    }
    else if(! _search_attempted)
    {
        out.push_back({ "Search", 5 });
    }
}

void Library::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::HallOfPortraits);
        break;

    case 2:
        screen.goto_room(RoomId::BedChamber01);
        break;

    case 3:
        _passage_found = true;
        screen.show_text("Pulling the book releases a hidden latch — a passage opens to the "
                          "north.");
        break;

    case 4:
    {
        _figurine_found = true;
        Result result = screen.roll(Attribute::INT_, 12, false, "Search for figurine");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::STRANGE_FIGURINE);
            screen.show_text("Tucked between two books, you find a strange carved figurine.");
        }
        else
        {
            screen.show_text("You find nothing more among the shelves.");
        }

        break;
    }

    case 5:
    {
        _search_attempted = true;
        Result result = screen.roll(Attribute::INT_, 10, false, "Search");

        if(result == Result::SUCCESS)
        {
            _book_found = true;
            screen.show_text("A false book spine catches your eye, slightly out of line "
                              "with the rest.");
        }
        else
        {
            screen.show_text("You find nothing out of the ordinary.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Library::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_search_attempted) bits |= 1 << 1;
    if(_book_found) bits |= 1 << 2;
    if(_passage_found) bits |= 1 << 3;
    if(_figurine_found) bits |= 1 << 4;
    return bits;
}

void Library::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _search_attempted = bits & (1 << 1);
    _book_found = bits & (1 << 2);
    _passage_found = bits & (1 << 3);
    _figurine_found = bits & (1 << 4);
}

}
