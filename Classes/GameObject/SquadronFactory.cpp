#include "SquadronFactory.h"

#include "Barrier.h"
#include "Blossom.h"
#include "Carrier.h"
#include "Snake.h"

USING_NS_CC;

Ship * SquadronFactory::createShipWithInfo(SquadronInfo info, int squadronID, int shipID)
{
    Ship* ship = nullptr;
    auto type = info.strings["type"];

    if (type == "Shield") {
        ship = Barrier::create(info, squadronID, shipID);
    }
    else if (type == "Blossom") {
        ship = Blossom::create(info, squadronID, shipID);
    }
    else if (type == "Carrier") {
        ship = Carrier::create(info, squadronID, shipID);
    }
    else if (type == "Serpent") {
        ship = Snake::create(info, squadronID, shipID);
    }
    else {
        ship = Ship::create(info, squadronID, shipID);
    }

    return ship;
}
