#include "escape.h"

#include "bn_vector.h"

#include "rng.h"

namespace sg
{

bool is_escaping()
{
    return enemy(EnemyId::Countess).is_dead();
}

bn::optional<EnemyId> pick_pursuer()
{
    bn::vector<EnemyId, int(EnemyId::COUNT)> pursuers;

    for(int i = 0; i < int(EnemyId::COUNT); ++i)
    {
        EnemyId id = EnemyId(i);

        if(id == EnemyId::Raven)
        {
            continue;
        }

        if(! enemy(id).is_dead())
        {
            pursuers.push_back(id);
        }
    }

    if(pursuers.empty())
    {
        return bn::optional<EnemyId>();
    }

    return pursuers[rng().get_int(0, pursuers.size())];
}

}
