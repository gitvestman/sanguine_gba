#include "rng.h"

namespace sg
{

bn::random& rng()
{
    static bn::random instance;
    return instance;
}

}
