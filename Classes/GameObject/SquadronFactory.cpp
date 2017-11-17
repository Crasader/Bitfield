#include "SquadronFactory.h"

#include "Ship.h"
#include "Barrier.h"
#include "Blossom.h"
#include "Carrier.h"
#include "Snake.h"
#include "Pulser.h"
#include "Unstable.h"

USING_NS_CC;

Ship* SquadronFactory::createShipWithInfo(World* world, SquadronInfo info, int squadronID, int shipID)
{
    Ship* ship = nullptr;
    auto type = info.strings["type"];

    if (type == "Shield") {
        ship = Barrier::create(world, info, squadronID, shipID);
    }
    else if (type == "Blossom") {
        ship = Blossom::create(world, info, squadronID, shipID);
    }
    else if (type == "Carrier") {
        ship = Carrier::create(world, info, squadronID, shipID);
    }
    else if (type == "Serpent") {
        ship = Snake::create(world, info, squadronID, shipID);
    }
    else if (type == "Pulser") {
        ship = Pulser::create(world, info, squadronID, shipID);
    }
    else if (type == "Unstable") {
        ship = Unstable::create(world, info, squadronID, shipID);
    }
    else {
        ship = Ship::create(world, info, squadronID, shipID);
    }

    return ship;
}
