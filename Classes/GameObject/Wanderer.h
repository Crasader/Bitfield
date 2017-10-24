#pragma once

#include "Ship.h"

class Wanderer : public Ship
{
public:
    Wanderer(SquadronInfo info);
    static Wanderer* create(SquadronInfo info);
};