#include "Carrier.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "GameObject\Bit.h"
#include "UI/World.h"
USING_NS_CC;

Carrier::Carrier(World* world, SquadronInfo info, int squadronID, int shipID)
    : Ship(world, info, squadronID, shipID)
{
    if (shipID > 0) {
        max_speed = info.doubles["mini_max_speed"];
        max_force = info.doubles["mini_max_force"];
        w_seek_bits = info.doubles["mini_w_seek_bits"];
        scale = info.doubles["mini_scale"];
        w_wander = 0;
        w_avoid_wall = 0;
        stopAllActions();
        runAction(EaseElasticOut::create(ScaleTo::create(1.5f, scale)));
        shouldReturn = false;
    }
    else {
        for (int i = 0; i < 7; i++) {
            int x = -30 + (10 * i);
            auto node = Node::create();
            node->setPositionX(x);
            addChild(node, 0, i);
        }
    }
}

Carrier* Carrier::create(World* world, SquadronInfo info, int squadronID, int shipID) {
    Carrier* ship = new (std::nothrow) Carrier(world, info, squadronID, shipID);
    
    // Use appropriate sprite for carrier vs minis
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
    if (shipID > 0) {
        auto leader = world->getSquadron(squadronID).at(0);
        if (shouldReturn && (leader->getPosition() - getPosition()).length() < separation_radius) {
            shouldReturn = false;
        }
    }
    Ship::update(delta);
}

cocos2d::Vec2 Carrier::cohesion()
{
    if (shipID == 0) return VEC_ZERO;
    auto ship = world->getSquadron(squadronID).at(0);
    auto node = ship->getChildByTag(shipID - 1);
    auto vel = ship->getVelocity().getNormalized();
    vel.scale(20);
    return seek(ship->getPosition() + node->getPosition() + vel, true);
}

cocos2d::Vec2 Carrier::seekBits() {
    if (shipID == 0 || shouldReturn) return VEC_ZERO;
    return Ship::seekBits();
}

void Carrier::onBitPickup()
{
    shouldReturn = true;
    Ship::onBitPickup();
}

bool Carrier::canSee(cocos2d::Node * target)
{
    return inRange(target);
}