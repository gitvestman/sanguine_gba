#include "great_chamber.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A grand chamber lined with portraits, each face turned slightly toward the door.";
    constexpr bn::string_view short_description = "The great chamber.";
}

void GreatChamber::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void GreatChamber::build_choices(bn::ivector<Choice>& out)
{
    if(_journal_reveal_step > 0)
    {
        out.push_back({ "Continue reading", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "W", 2 });
    out.push_back({ "Inspect paintings", 3 });

    if(! _found_journal)
    {
        out.push_back({ "Search", 4 });
    }

    if(Inventory::instance().has(ItemId::JOURNAL) && ! _read_journal)
    {
        out.push_back({ "Read journal", 5 });
    }
}

void GreatChamber::on_choice(int action_id, RoomScreen& screen)
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
        switch(_journal_reveal_step)
        {

        case 1:
            Inventory::instance().add(ItemId::SHEET_MUSIC);
            _journal_reveal_step = 2;
            screen.show_text("The next page describes a folded sheet of music, tucked "
                              "inside the journal's back cover. You find it there.");
            break;

        case 2:
            Inventory::instance().add(ItemId::JOURNAL);
            _journal_reveal_step = 0;
            screen.show_text("The last page trails into frantic, illegible scrawl. You "
                              "pocket the journal itself.");
            break;

        default:
            _journal_reveal_step = 0;
            break;
        }

        break;

    case 2:
        screen.goto_room(RoomId::FancyCorridor01);
        break;

    case 3:
        screen.show_text("Every portrait's eyes seem to follow you as you pass.");
        break;

    case 4:
    {
        _found_journal = true;
        Result result = screen.roll(Attribute::INT_, 10, true, "Search");

        if(result == Result::SUCCESS)
        {
            _journal_reveal_step = 1;
            screen.show_text("Behind a portrait, a hidden journal falls open. The first "
                              "page describes this very room.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("You find nothing behind the portraits.");
        }

        break;
    }

    case 5:
        _read_journal = true;
        screen.show_text("The journal's owner wrote of a growing dread, and of a woman who "
                          "never seemed to age.");
        break;

    default:
        break;
    }
}

uint16_t GreatChamber::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_found_journal) bits |= 1 << 1;
    if(_read_journal) bits |= 1 << 2;
    bits |= (uint16_t(_journal_reveal_step) & 0x3) << 3;
    return bits;
}

void GreatChamber::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _found_journal = bits & (1 << 1);
    _read_journal = bits & (1 << 2);
    _journal_reveal_step = (bits >> 3) & 0x3;
}

}
