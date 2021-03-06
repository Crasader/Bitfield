#pragma once

#include "Ship.h"

class Blossom : public Ship
{
public:
    static Blossom* create(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void update(float delta) override;

    // Petals seek a node around the center
    cocos2d::Vec2 cohesion() override;

    // Only the petals seek bits
    cocos2d::Vec2 seekBits() override;

    virtual cocos2d::Color3B getStreakColor() override;

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void loadStreak(SquadronInfo& info);

private:
    int inner_radius_min;
    int inner_radius_max;
    int outer_radius_min;
    int outer_radius_max;
    int vel_inner;
    int vel_outer;

    void addPetalNodes();
};  