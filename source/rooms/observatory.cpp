#include "observatory.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A domed observatory, its brass telescope pointed at a gap in the shattered roof.";
    constexpr bn::string_view short_description = "The observatory.";
}

void Observatory::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Observatory::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "Search", 1 });
    out.push_back({ "Study the zodiac chart", 2 });
    out.push_back({ "Use telescope", 3 });
    out.push_back({ "Go back", 4 });
}

void Observatory::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("Star charts and dusty instruments, none of it making sense to "
                          "you.");
        break;

    case 2:
        screen.show_text("The zodiac chart is annotated in a spidery hand, marking dates "
                          "you don't recognize.");
        break;

    case 3:
        screen.show_text("Through the telescope, the night sky looks wrong somehow - the "
                          "stars in the wrong places entirely.");
        break;

    case 4:
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    default:
        break;
    }
}

}
