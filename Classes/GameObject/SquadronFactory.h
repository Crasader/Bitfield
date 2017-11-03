#pragma once

#include "Ship.h"

class SquadronFactory
{
public:
    static Ship* createShipWithInfo(SquadronInfo info, int squadronID, int shipID);
};