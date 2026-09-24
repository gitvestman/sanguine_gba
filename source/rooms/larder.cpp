#include "larder.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "kitchen_cellar.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A dank larder stacked with barrels, the air thick with the smell of spoiled meat.";
    constexpr bn::string_view short_description = "The larder.";
}

void Larder::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Larder::build_choices(bn::ivector<Choice>& out)
{
    if(_barrels_opened && ! _steeled)
    {
        out.push_back({ "Steel yourself", 3 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "Go back", 1 });

    if(! _searched)
    {
        out.push_back({ "Search", 4 });
    }

    if(! _barrels_opened)
    {
        out.push_back({ "Look in barrels", 2 });
    }
}

void Larder::on_choice(int action_id, RoomScreen& screen)
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
        static_cast<KitchenCellar&>(room(RoomId::KitchenCellar)).announce_rat_if_visited();
        screen.goto_room(RoomId::KitchenCellar);
        break;

    case 2:
        _barrels_opened = true;
        screen.show_text("You pry open a barrel. A bloated, half-formed shape lies curled "
                          "inside, pickled in brine.");
        break;

    case 3:
    {
        Result result = screen.roll(Attribute::STR, 10, false, "Steel yourself");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You force down your revulsion and turn away.");
        }
        else
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("Your stomach turns. You lose 1 Sanity.");
        }

        _steeled = true;
        break;
    }

    case 4:
        _searched = true;
        screen.show_text("Moldy hams hang from the rafters. Nothing worth taking.");
        break;

    default:
        break;
    }
}

uint8_t Larder::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_barrels_opened) bits |= 1 << 2;
    if(_steeled) bits |= 1 << 3;
    return bits;
}

void Larder::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _barrels_opened = bits & (1 << 2);
    _steeled = bits & (1 << 3);
}

}
