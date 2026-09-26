#include "reliquary.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Rows of glass reliquaries line the walls, each holding a yellowed bone fragment "
        "behind candlelight.";
    constexpr bn::string_view short_description = "The reliquary.";
    constexpr bn::string_view inspecting_hint = "\n\nOne reliquary's glass looks freshly "
                                                "cracked. Something about it feels wrong.";
}

void Reliquary::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_inspecting && ! _trap_sprung)
    {
        out.append(inspecting_hint);
    }
}

void Reliquary::build_choices(bn::ivector<Choice>& out)
{
    if(_inspecting && ! _trap_sprung)
    {
        out.push_back({ "Ignore your intuition", 1 });
        out.push_back({ "Step back", 2 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "W", 3 });

    if(! _searched)
    {
        out.push_back({ "Search", 4 });
    }

    if(! _trap_sprung)
    {
        out.push_back({ "Inspect the cracked glass", 5 });
    }
}

void Reliquary::on_choice(int action_id, RoomScreen& screen)
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
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Ignore your intuition");
        _trap_sprung = true;
        _inspecting = false;

        if(result == Result::SUCCESS)
        {
            screen.show_text("You brush the warning aside, and nothing comes of it.");
        }
        else
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("The glass shatters and something unseen brushes past your "
                              "face. You lose 1 Sanity.");
        }

        break;
    }

    case 2:
        _inspecting = false;
        screen.show_text("You step back, unwilling to test your luck.");
        break;

    case 3:
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    case 4:
        _searched = true;
        screen.show_text("Row upon row of bone fragments, each labeled in faded Latin.");
        break;

    case 5:
        _inspecting = true;
        screen.show_text("One reliquary's glass is cracked, as if something inside had "
                          "pushed against it.");
        break;

    default:
        break;
    }
}

void Reliquary::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, -75, -46 });
    out.push_back({ AmbientKind::CANDLE, -65, -46 });
    out.push_back({ AmbientKind::CANDLE, -55, -46 });
    out.push_back({ AmbientKind::CANDLE, 55, -46 });
    out.push_back({ AmbientKind::CANDLE, 65, -46 });
    out.push_back({ AmbientKind::CANDLE, 75, -46 });
}

uint16_t Reliquary::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_inspecting) bits |= 1 << 2;
    if(_trap_sprung) bits |= 1 << 3;
    return bits;
}

void Reliquary::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _inspecting = bits & (1 << 2);
    _trap_sprung = bits & (1 << 3);
}

}
