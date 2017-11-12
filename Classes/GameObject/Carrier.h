#pragma once

#include "Ship.h"

class Carrier : public Ship
{
public:
    Carrier(SquadronInfo info, int squadronID, int shipID);
    static Carrier* create(SquadronInfo info, int squadronID, int shipID);

    virtual void update(float delta) override;

    // Mini ships try to stay inside the carrier
    cocos2d::Vec2 cohesion() override;

    // Mini ships seek bits
    cocos2d::Vec2 seekBits() override;

private:
};  