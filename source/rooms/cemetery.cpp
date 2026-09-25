#include "cemetery.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

#include "bn_sprite_items_npc_myling.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Gravestones crowd this fog-slicked plot behind a rusted iron fence. The road forks "
        "four ways.";
    constexpr bn::string_view short_description = "The cemetery.";
}

void Cemetery::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Cemetery::_add_name_choices(bn::ivector<Choice>& out) const
{
    out.push_back({ "\"Hans?\"", 20 });
    out.push_back({ "\"Father?\"", 21 });
    out.push_back({ "\"Ahmed?\"", 22 });
    out.push_back({ "My own name", 23 });
    out.push_back({ "\"Jose?\"", 24 });
    out.push_back({ "\"Karl?\"", 25 });
}

void Cemetery::build_choices(bn::ivector<Choice>& out)
{
    if(_myling_resolved || ! _investigation_started)
    {
        out.push_back({ "Look", 0 });
        out.push_back({ "N", 1 });
        out.push_back({ "E", 2 });
        out.push_back({ "S", 3 });
        out.push_back({ "W", 4 });
    }

    if(_myling_resolved)
    {
        return;
    }

    if(! _investigation_started)
    {
        if(! _searched)
        {
            out.push_back({ "Search", 5 });
        }
        else
        {
            out.push_back({ "Investigate the sound", 6 });
            out.push_back({ "Ignore it", 7 });
        }

        return;
    }

    // Investigation started — movement is gone until this resolves.
    if(_approached == MylingApproach::NONE)
    {
        out.push_back({ "Announce yourself", 8 });
        out.push_back({ "Move closer", 9 });
    }
    else if(_approached == MylingApproach::CLOSE_SUCCESS)
    {
        if(_understood == MylingTriState::NONE)
        {
            out.push_back({ "Back away", 10 });
            out.push_back({ "Try to understand", 11 });
        }
        else if(! _spoke_to_myling)
        {
            out.push_back({ "Back away", 10 });
            out.push_back({ "Speak to it", 12 });
        }
        else
        {
            _add_name_choices(out);
        }
    }
    else
    {
        // ANNOUNCED or CLOSE_FAIL
        if(_horror_withstood == MylingTriState::NONE)
        {
            out.push_back({ "Withstand the horror", 13 });
        }
        else if(_horror_withstood == MylingTriState::SUCCESS)
        {
            _add_name_choices(out);
        }
        else
        {
            out.push_back({ "Run", 14 });
            _add_name_choices(out);
        }
    }
}

void Cemetery::_resolve_myling(RoomScreen& screen)
{
    _myling_resolved = true;
    screen.hide_overlay_sprite();
}

void Cemetery::on_enter(RoomScreen& screen)
{
    if(_investigation_started && ! _myling_resolved)
    {
        screen.show_overlay_sprite(bn::sprite_items::npc_myling);
    }
}

void Cemetery::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::CliffTombs);
        break;

    case 2:
        screen.goto_room(RoomId::Tombstones);
        break;

    case 3:
        screen.goto_room(RoomId::OvergrownPond);
        break;

    case 4:
        screen.goto_room(RoomId::Mausoleum);
        break;

    case 5:
        _searched = true;
        screen.show_text("Among the graves, you notice a small, half-buried shape stirring "
                          "in the fog.");
        break;

    case 6:
        _investigation_started = true;
        screen.show_overlay_sprite(bn::sprite_items::npc_myling);
        screen.show_text("A soft, wet sobbing rises from among the stones. Something small "
                          "moves between them.");
        break;

    case 7:
        _resolve_myling(screen);
        screen.show_text("You decide it's best not to know. You walk on.");
        break;

    case 8:
        _approached = MylingApproach::ANNOUNCED;
        screen.show_text("You call out. The sobbing stops. Something small turns toward you.");
        break;

    case 9:
    {
        Result result = screen.roll(Attribute::DEX, 10, false, "Move closer");

        if(result == Result::SUCCESS)
        {
            _approached = MylingApproach::CLOSE_SUCCESS;
            screen.show_text("You creep closer without a sound. It's a child, or something "
                              "wearing a child's shape.");
        }
        else
        {
            _approached = MylingApproach::CLOSE_FAIL;
            screen.show_text("A twig snaps underfoot. It snaps toward you with unnatural "
                              "speed.");
        }

        break;
    }

    case 10:
    {
        Result result = screen.roll(Attribute::DEX, 10, false, "Back away");

        if(result == Result::SUCCESS)
        {
            _resolve_myling(screen);
            screen.show_text("You retreat quietly, and the small shape loses interest.");
        }
        else
        {
            _spoke_to_myling = true;
            screen.show_text("It lunges, gripping your sleeve with a cold, small hand. It "
                              "won't let go until you speak.");
        }

        break;
    }

    case 11:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Try to understand");

        if(result == Result::SUCCESS)
        {
            _understood = MylingTriState::SUCCESS;
            screen.show_text("Between sobs, you make out a plea: it wants to be called by "
                              "its name.");
        }
        else
        {
            _understood = MylingTriState::FAIL;
            screen.show_text("Its words are just wind and grief. You understand nothing.");
        }

        break;
    }

    case 12:
        _spoke_to_myling = true;
        screen.show_text("You kneel and speak gently to it. It stares back, waiting.");
        break;

    case 13:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Withstand the horror");

        if(result == Result::SUCCESS)
        {
            _horror_withstood = MylingTriState::SUCCESS;
            screen.show_text("You force yourself to look upon it without flinching.");
        }
        else
        {
            _horror_withstood = MylingTriState::FAIL;
            GameState::instance().change_sanity(-1);
            screen.show_text("Its hollow eyes are too much to bear. You lose 1 Sanity.");
        }

        break;
    }

    case 14:
        GameState::instance().take_damage(1);
        _resolve_myling(screen);
        screen.show_text("You turn and run, small cold fingers grazing your ankle as you "
                          "go. You take 1 damage.");
        break;

    case 23:
        GameState::instance().change_luck(1);
        _resolve_myling(screen);
        screen.show_text("At your own name, the small shape stills, then dissolves into the "
                          "fog, at peace. You feel luckier for it.");
        break;

    case 20:
    case 21:
    case 22:
    case 24:
    case 25:
        GameState::instance().take_damage(1);
        _resolve_myling(screen);
        screen.show_text("Wrong. It shrieks and claws at you before vanishing into the fog. "
                          "You take 1 damage.");
        break;

    default:
        break;
    }
}

uint16_t Cemetery::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_myling_resolved) bits |= 1 << 1;
    if(_investigation_started) bits |= 1 << 2;
    if(_searched) bits |= 1 << 3;
    if(_spoke_to_myling) bits |= 1 << 4;
    bits |= (uint16_t(_approached) & 0x3) << 5;
    bits |= (uint16_t(_understood) & 0x3) << 7;
    bits |= (uint16_t(_horror_withstood) & 0x3) << 9;
    return bits;
}

void Cemetery::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _myling_resolved = bits & (1 << 1);
    _investigation_started = bits & (1 << 2);
    _searched = bits & (1 << 3);
    _spoke_to_myling = bits & (1 << 4);
    _approached = MylingApproach((bits >> 5) & 0x3);
    _understood = MylingTriState((bits >> 7) & 0x3);
    _horror_withstood = MylingTriState((bits >> 9) & 0x3);
}

}
