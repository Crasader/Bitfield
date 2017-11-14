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

    cocos2d::Vec2 seekBits() override;

    virtual void onBitPickup() override;

    virtual bool canSee(cocos2d::Node* target) override;

private:
    bool shouldReturn;
};  