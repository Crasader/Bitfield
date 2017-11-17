#pragma once

#include "cocos2d.h"
#include "Types.h"

class Ship;

class ShipStreak : public cocos2d::MotionStreak
{
public:
    ShipStreak();
    static ShipStreak* create(Ship* ship, StreakInfo info);

    bool init(Ship* ship, StreakInfo info);

    void update(float delta) override;

private:
    Ship* ship;
};