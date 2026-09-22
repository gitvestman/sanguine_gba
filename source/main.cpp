#include "bn_core.h"

#include "ui/room_screen.h"

int main()
{
    bn::core::init();

    sg::RoomScreen screen;
    screen.run(sg::RoomId::CellarHall);
}
