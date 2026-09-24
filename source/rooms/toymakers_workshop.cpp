#include "toymakers_workshop.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Shelves of half-finished toys line this cramped workshop, candlelight glinting off "
        "glass eyes.";
    constexpr bn::string_view short_description = "The toymaker's workshop.";
}

void ToymakersWorkshop::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void ToymakersWorkshop::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "Down (closet stairs)", 2 });

    if(! _searched)
    {
        out.push_back({ "Search", 3 });
    }

    if(! _toy_built)
    {
        out.push_back({ "Build a toy", 4 });
    }
}

void ToymakersWorkshop::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    case 2:
        screen.goto_room(RoomId::ChildrensRoom);
        break;

    case 3:
        _searched = true;
        screen.show_text("Spare parts, half-carved wood, and rows of glass eyes watching "
                          "from a jar.");
        break;

    case 4:
    {
        _toy_built = true;
        Result result = screen.roll(Attribute::DEX, 10, false, "Build a toy");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::TOY_SOLDIER);
            screen.show_text("Piecing together spare parts, you assemble a small clockwork "
                              "toy soldier.");
        }
        else
        {
            screen.show_text("The pieces won't fit together right. You give up.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t ToymakersWorkshop::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_toy_built) bits |= 1 << 2;
    return bits;
}

void ToymakersWorkshop::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _toy_built = bits & (1 << 2);
}

}
