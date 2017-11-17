#pragma once

#include "Ship.h"

class Unstable : public Ship
{
public:
    static Unstable* create(World* world, SquadronInfo& info, int squadronID, int shipID);

    virtual void update(float delta) override;

    cocos2d::Color3B getStreakColor() override;

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID) override;

private:
    void igniteRandomTile();

    int flames;
    double timer;
    double flame_t;
};  