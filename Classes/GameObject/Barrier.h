#pragma once

#include "Ship.h"

class Barrier : public Ship
{
public:
    Barrier(SquadronInfo info, int squadronID, int shipID);
    static Barrier* create(SquadronInfo info, int squadronID, int shipID);
    virtual void update(float delta) override;

    virtual void onBitPickup() override;
    virtual void handleCollisions() override;

private:
    double start_scale;
    double shrink_scale;
    double grow_scale;
    double max_scale;

    float target_scale;
};  