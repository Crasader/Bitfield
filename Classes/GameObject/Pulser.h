#pragma once

#include "Ship.h"

class Pulser : public Ship
{
public:
    static Pulser* create(World* world, SquadronInfo& info, int squadronID, int shipID);
    bool init(World* world, SquadronInfo& info, int squadronID, int shipID) override;

    cocos2d::Color3B getStreakColor() override;
    int target_x, target_y;

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void calculateForces(float delta) override;

private:
    void createEventListener();

    static std::vector<int> number_reached_target;
    bool reached_target;

    double timer;
    double pulse_t;
};  