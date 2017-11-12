#include "Carrier.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
USING_NS_CC;

Carrier::Carrier(SquadronInfo info, int squadronID, int shipID)
    : Ship(info, squadronID, shipID)
{
    if (shipID == 0) {
        
    }
    else {
        scale = 0.5f;
    }
}

Carrier* Carrier::create(SquadronInfo info, int squadronID, int shipID) {
    Carrier* ship = new (std::nothrow) Carrier(info, squadronID, shipID);
    
    // Use appropriate sprite for body vs petals
    std::string path;
    if (shipID == 0) path = info.strings["sprite"];
    else path = info.strings["sprite_mini"];
    
    if (ship && ship->initWithFile(path)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Carrier::update(float delta)
{
    Ship::update(delta);
}

cocos2d::Vec2 Carrier::cohesion()
{
    if (shipID == 0) return Vec2(0, 0);
    auto ship = neighbours->at(0);
    return seek(ship->getPosition());
}

cocos2d::Vec2 Carrier::seekBits() {
    if (shipID == 0) return Ship::seekBits();
    return VEC_ZERO;
}