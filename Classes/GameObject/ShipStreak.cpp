#include "ShipStreak.h"
#include "Ship.h"

USING_NS_CC;

ShipStreak::ShipStreak() {
    ship = nullptr;
}

ShipStreak* ShipStreak::create(Ship* ship, StreakInfo info)
{
    ShipStreak *ret = new (std::nothrow) ShipStreak();
    if (ret && ret->init(ship, info))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ShipStreak::init(Ship* ship, StreakInfo info)
{
    if (!MotionStreak::initWithFade(info.fade, info.minSeg, info.stroke, info.color, info.path)) return false;
    this->ship = ship;
    scheduleUpdate();
    return true;
}

void ShipStreak::update(float delta)
{
    auto parent = ship->getParent();
    setPosition(ship->getPosition());
    MotionStreak::update(delta);
}

