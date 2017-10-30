#pragma once

#include "Ship.h"

class Wanderer : public Ship
{
public:
    Wanderer(SquadronInfo info);
    static Wanderer* Wanderer::create(SquadronInfo info);
};