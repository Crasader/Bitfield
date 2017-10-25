#include "SquadronFactory.h"

#include "Wanderer.h"


USING_NS_CC;

Ship * SquadronFactory::createShipWithInfo(SquadronInfo info, int squadronID, int shipID)
{
    Ship* ship = nullptr;
    auto type = info.strings["type"];

    if (type == "Wanderer") {
        ship = Ship::create(info, squadronID, shipID);
    }

    return ship;
}
