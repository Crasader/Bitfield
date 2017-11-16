#pragma once

#include "Ship.h"

class Snake : public Ship
{
public:
    Snake(SquadronInfo info, int squadronID, int shipID);
    static Snake* create(SquadronInfo info, int squadronID, int shipID);

    // Head does not avoid other body parts
    cocos2d::Vec2 separate() override;
    
    // Segments follow the next segment in the chain
    cocos2d::Vec2 cohesion() override;

    // Only the head seeks bits and wanders
    cocos2d::Vec2 seekBits() override;
    cocos2d::Vec2 wander() override;
};  