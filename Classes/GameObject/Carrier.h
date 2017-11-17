#pragma once

#include "Ship.h"

class Carrier : public Ship
{
public:
    static Carrier* create(World* world, SquadronInfo& info, int squadronID, int shipID);

    virtual void update(float delta) override;

    // Mini ships try to stay inside the carrier
    cocos2d::Vec2 cohesion() override;

    // Only mini ships seek bits
    cocos2d::Vec2 seekBits() override;

    // Carrier can see 360 degrees
    virtual bool canSee(cocos2d::Node* target) override;

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);
    
    // Mini ships return to Carrier after pickup
    virtual void onBitPickup() override;

private:
    bool shouldReturn;
};  