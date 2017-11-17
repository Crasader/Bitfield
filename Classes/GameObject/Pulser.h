#pragma once

#include "Ship.h"

class Pulser : public Ship
{
public:
    Pulser(World* world, SquadronInfo info, int squadronID, int shipID);
    static Pulser* create(World* world, SquadronInfo info, int squadronID, int shipID);
    virtual bool initWithFile(const std::string& path) override;

    virtual void calculateForces(float delta) override;

    int target_x, target_y;

private:
    void createEventListener();

    static std::vector<int> number_reached_target;
    bool reached_target;

    double timer;
    double pulse_t;
};  