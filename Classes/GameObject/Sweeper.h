#pragma once

#include "Ship.h"

class Sweeper : public Ship
{
public:
    static Sweeper* create(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void update(float delta) override;

    cocos2d::Color3B getStreakColor() override;
    int target_x, target_y;

protected:
    void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);
    void calculateForces(float delta) override;
    void onBitPickup() override;

private:
    void getNextTarget();

    double timer;
    double pulse_t;
};  