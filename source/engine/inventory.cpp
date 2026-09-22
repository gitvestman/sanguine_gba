#include "inventory.h"

namespace sg
{

Inventory& Inventory::instance()
{
    static Inventory inventory;
    return inventory;
}

}
