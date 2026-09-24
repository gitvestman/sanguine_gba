#include "ghostly_bride.h"

#include "../engine/game_state.h"

namespace sg::dialogue
{

namespace
{
    enum ChoiceId
    {
        ADDRESS,
        APPROACH,
        WAIT,
        WHO_ARE_YOU,
        WHAT_HAPPENED,
        GESTURE_THROAT,
        HOLD_HANDS,
        WHAT_BINDS,
        TRY_UNDERSTAND,
        CALM
    };
}

void GhostlyBride::initialize_choices()
{
    add_dialogue(ADDRESS, "Address her", "You clear your throat. Her head snaps toward you.");
    add_dialogue(APPROACH, "Approach her",
                 "She is young, no more than sixteen, dressed in clothes centuries out of "
                 "fashion.");
    add_dialogue(WAIT, "Wait and listen",
                 "She prays in an old tongue. You catch a word: forgiveness.");

    add_dialogue(WHO_ARE_YOU, "\"Who are you?\"",
                 "She answers in a rush of foreign words. You catch a name: Catalina.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(WHAT_HAPPENED, "\"What happened to you?\"",
                 "She stands, tears welling, hand at her throat. Her words come too fast to "
                 "follow.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(GESTURE_THROAT, "Gesture toward her throat",
                 "She stands, tears welling, hand at her throat. Her words come too fast to "
                 "follow.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(HOLD_HANDS, "Hold up both hands",
                 "She quiets, watching you with eyes like pools of shadow and old fear.",
                 DialogueChoiceType::ONCE, true);

    add_quit(WHAT_BINDS, "\"What binds you here?\"",
             "She mumbles of a mother, and a fire. Then an unseen force wrenches her away "
             "through the ceiling.", true);
    add_quit(TRY_UNDERSTAND, "Try to understand her",
             "You focus, trying to understand her.", true);
    add_quit(CALM, "\"No, wait. Calm yourself!\"",
             "She grows frantic. An unseen force wrenches her away through the ceiling.", true);
}

void GhostlyBride::on_choice_selected(int id, RoomScreen& screen)
{
    switch(id)
    {

    case ADDRESS:
        hide_dialogue(APPROACH);
        hide_dialogue(WAIT);
        show_dialogue(WHO_ARE_YOU);
        show_dialogue(WHAT_HAPPENED);
        show_dialogue(GESTURE_THROAT);
        show_dialogue(HOLD_HANDS);
        break;

    case WHO_ARE_YOU:
        hide_dialogue(GESTURE_THROAT);
        hide_dialogue(HOLD_HANDS);
        show_dialogue(WHAT_BINDS);
        show_dialogue(WHAT_HAPPENED);
        break;

    case HOLD_HANDS:
        hide_dialogue(WHO_ARE_YOU);
        show_dialogue(WHAT_BINDS);
        show_dialogue(GESTURE_THROAT);
        break;

    case WHAT_HAPPENED:
    case GESTURE_THROAT:
        hide_dialogue(WHO_ARE_YOU);
        hide_dialogue(HOLD_HANDS);
        hide_dialogue(WHAT_HAPPENED);
        hide_dialogue(GESTURE_THROAT);
        show_dialogue(TRY_UNDERSTAND);
        show_dialogue(CALM);
        break;

    case TRY_UNDERSTAND:
    {
        Result result = screen.roll(Attribute::INT_, 12, false, "Understand her");

        if(result == Result::SUCCESS)
        {
            screen.show_text("A single word repeats: mother. A chilling revelation dawns on "
                              "you as she is wrenched away through the ceiling.");
        }
        else
        {
            screen.show_text("She grows frantic as you fail to follow. An unseen force "
                              "wrenches her away through the ceiling.");
        }

        _departed = true;
        break;
    }

    case WHAT_BINDS:
    case CALM:
        _departed = true;
        break;

    default:
        break;
    }
}

}
