#include "Snake.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "UI\World.h"
USING_NS_CC;

Snake* Snake::create(World* world, SquadronInfo& info, int squadronID, int shipID) {
    Snake* ship = new (std::nothrow) Snake();
    if (ship && ship->init(world, info, squadronID, shipID)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Snake::loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID) {
    Ship::loadInfo(world, info, squadronID, shipID);

    // Use appropriate sprite for body vs tail
    if (shipID == info.ints["count"] - 1)
        sprite = info.strings["sprite_tail"];
    else
        sprite = info.strings["sprite"];
}

cocos2d::Vec2 Snake::separate() {
    if (shipID == 0) return VEC_ZERO;
    return Ship::separate();
}

cocos2d::Vec2 Snake::cohesion()
{
    if (shipID == 0) return VEC_ZERO;
    auto nextShip = world->getSquadron(squadronID).at(shipID - 1);
    return seek(nextShip->getPosition(), true);
}

cocos2d::Vec2 Snake::seekBits() {
    if (shipID == 0) return Ship::seekBits();
    return VEC_ZERO;
}

cocos2d::Vec2 Snake::wander()
{
    if (shipID == 0) return Ship::wander(); 
    return VEC_ZERO;
}

