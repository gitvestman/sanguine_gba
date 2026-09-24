#include "golem.h"

#include "../engine/combat.h"

namespace sg::dialogue
{

namespace
{
    enum ChoiceId
    {
        ATTACK,
        BACK_OFF
    };
}

void Golem::initialize_choices()
{
    add_quit(ATTACK, "Attack it", "You strike before it can react.");
    add_quit(BACK_OFF, "Back away", "You step back. The golem watches, motionless, but "
                                    "doesn't pursue.");
}

void Golem::on_choice_selected(int id, RoomScreen& screen)
{
    switch(id)
    {

    case ATTACK:
        Combat::fight(screen, EnemyId::Golem);
        break;

    case BACK_OFF:
        _resolved = true;
        break;

    default:
        break;
    }
}

}
