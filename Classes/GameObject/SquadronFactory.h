#pragma once

#include "Types.h"

class Ship;
class World;

class SquadronFactory
{
public:
    static Ship* createShipWithInfo(World* world,
        SquadronInfo info, int squadronID, int shipID);
};