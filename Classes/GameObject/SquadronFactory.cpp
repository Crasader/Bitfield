#include "SquadronFactory.h"

#include "Snake.h"


USING_NS_CC;

Ship * SquadronFactory::createShipWithInfo(SquadronInfo info, int squadronID, int shipID)
{
    Ship* ship = nullptr;
    auto type = info.strings["type"];

    if (type == "Snake") {
        ship = Snake::create(info, squadronID, shipID);
    }
    else {
        ship = Ship::create(info, squadronID, shipID);
    }

    return ship;
}
