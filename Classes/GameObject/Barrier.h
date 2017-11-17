#pragma once

#include "Ship.h"

class Barrier : public Ship
{
public:
    static Barrier* create(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void update(float delta) override;

    virtual void onBitPickup() override;

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);

private:
    double start_scale;
    double shrink_scale;
    double grow_scale;
    double max_scale;

    float target_scale;
};  