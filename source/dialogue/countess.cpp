#include "countess.h"

#include "../engine/combat.h"

namespace sg::dialogue
{

namespace
{
    enum ChoiceId
    {
        GREET_POLITE,
        GREET_RUDE,
        GREET_SILENT,
        ATTACK,
        GIVE_NAME,
        ASK_QUESTION,
        LEAVE,
        DINNER_ACCEPT,
        DINNER_DECLINE,
        LET_PASS,
        ATTACK_BEHIND
    };
}

void Countess::initialize_choices()
{
    add_dialogue(GREET_POLITE, "\"Good evening, my Lady.\"",
                 "\"Good evening, traveler. How rare, a visitor at this hour.\"");
    add_dialogue(GREET_RUDE, "\"Who are you?\"",
                 "Her expression cools. \"You forget your manners. I am the mistress of "
                 "this house.\"");
    add_dialogue(GREET_SILENT, "Stay silent",
                 "She studies you in the silence, unbothered. \"Not one for words, I see.\"");
    add_quit(ATTACK, "Attack her", "You lunge without warning.");

    add_dialogue(GIVE_NAME, "Give your name", "\"A pleasure,\" she says, though her smile "
                                              "doesn't reach her eyes.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(ASK_QUESTION, "Ask about the castle",
                 "\"This place has stood for centuries, and will stand for many more.\" She "
                 "watches you a moment too long.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(LEAVE, "Take your leave",
                 "\"Already? Nonsense. You'll dine with me before you go anywhere.\"",
                 DialogueChoiceType::ONCE, true);
    add_quit(DINNER_ACCEPT, "Accept the invitation",
             "\"Wonderful,\" she says, already turning toward the dining hall.", true);
    add_dialogue(DINNER_DECLINE, "\"Respectfully, I must decline.\"",
                 "\"A pity.\" She studies you a moment longer.",
                 DialogueChoiceType::ONCE, true);
    add_quit(LET_PASS, "Let her go", "You lower your guard and let her pass.", true);
    add_quit(ATTACK_BEHIND, "Attack from behind", "You strike as she turns away.", true);
}

void Countess::on_choice_selected(int id, RoomScreen& screen)
{
    switch(id)
    {

    case GREET_POLITE:
    case GREET_RUDE:
    case GREET_SILENT:
        show_dialogue(GIVE_NAME);
        break;

    case GIVE_NAME:
        show_dialogue(ASK_QUESTION);
        show_dialogue(LEAVE);
        break;

    case LEAVE:
        show_dialogue(DINNER_ACCEPT);
        show_dialogue(DINNER_DECLINE);
        break;

    case DINNER_ACCEPT:
        _dinner_pending = true;
        break;

    case DINNER_DECLINE:
        show_dialogue(LET_PASS);
        show_dialogue(ATTACK_BEHIND);
        break;

    case LET_PASS:
        _resolved = true;
        break;

    case ATTACK:
    case ATTACK_BEHIND:
        // Only her FancyCorridor01 fight has this limit (countess_hostile.gd's
        // check_corridor_flee() checks the current room) — DinnerHall's own
        // "Attack her" calls Combat::fight directly, without this parameter,
        // so that one stays a real fight to the death.
        if(Combat::fight(screen, EnemyId::Countess, 2) == CombatResult::ENEMY_FLED)
        {
            _hiding_in_guest_bedroom = true;
        }

        break;

    default:
        break;
    }
}

}
