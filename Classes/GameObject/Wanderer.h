#pragma once

#include "Ship.h"

class Wanderer : public Ship
{
public:
    static Wanderer* Wanderer::create();
};