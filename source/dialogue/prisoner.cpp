#include "prisoner.h"

#include "../engine/inventory.h"
#include "../engine/game_state.h"

namespace sg::dialogue
{

namespace
{
    enum ChoiceId
    {
        WHO_ARE_YOU,
        WHO_IMPRISONED,
        HELP,
        DRESS_WOUNDS,
        GIVE_TINCTURE
    };
}

void Prisoner::initialize_choices()
{
    add_dialogue(WHO_ARE_YOU, "Who are you?",
                 "\"Isaac. Bastian brought me here chasing his family's secrets.\"");
    add_dialogue(WHO_IMPRISONED, "Who did this?",
                 "\"I dare not say her name. She rules this keep, and toys with all who enter.\"");
    add_dialogue(HELP, "I'm here to help.",
                 "Tears well in his eyes. He is malnourished and wounded.");
}

void Prisoner::on_choice_selected(int id)
{
    switch(id)
    {

    case HELP:
        add_quit(DRESS_WOUNDS, "Dress his wounds",
                 "You bind his wounds with what you have. It will have to do for now.");

        if(Inventory::instance().has(ItemId::HEALING_TINCTURE))
        {
            add_quit(GIVE_TINCTURE, "Give him the tincture (-1, LCK+1)",
                     "He drinks it gratefully, and some strength returns to his eyes.");
        }

        break;

    case DRESS_WOUNDS:
        _resolved = true;
        break;

    case GIVE_TINCTURE:
        Inventory::instance().remove(ItemId::HEALING_TINCTURE);
        GameState::instance().change_luck(1);
        _resolved = true;
        break;

    default:
        break;
    }
}

}
