#pragma once

#include "Ship.h"

class Unstable : public Ship
{
public:
    Unstable(World* world, SquadronInfo info, int squadronID, int shipID);
    static Unstable* create(World* world, SquadronInfo info, int squadronID, int shipID);

    virtual void update(float delta) override;

    void igniteRandomTile();

private:
    int flames;
    int target_x, target_y;

    double timer;
    double flame_t;
};  