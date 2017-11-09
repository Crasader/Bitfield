#include "Blossom.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
USING_NS_CC;

Blossom::Blossom(SquadronInfo info, int squadronID, int shipID)
    : Ship(info, squadronID, shipID)
{
    inner_radius_min = info.ints["inner_radius_min"];
    inner_radius_max = info.ints["inner_radius_max"];
    outer_radius_min = info.ints["outer_radius_min"];
    outer_radius_max = info.ints["outer_radius_max"];
    vel_inner = info.ints["vel_inner"];
    vel_outer = info.ints["vel_outer"];
    point_to_velocity = false;

    if (shipID == 0) {
        addPetalNodes();
    }
}

Blossom* Blossom::create(SquadronInfo info, int squadronID, int shipID) {
    Blossom* ship = new (std::nothrow) Blossom(info, squadronID, shipID);
    
    // Use appropriate sprite for body vs petals
    std::string path;
    if (shipID == 0) path = info.strings["sprite"];
    else path = info.strings["sprite_petal"];
    
    if (ship && ship->initWithFile(path)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Blossom::update(float delta)
{
    // Update petal positions
    if (shipID == 0) {
        auto offset = getChildByName("offset");
        for (int i = 0; i < 7; i++) {
            auto petalNode = getChildByTag(i);
            auto pos = petalNode->getPosition();
            pos.rotate(VEC_ZERO, CC_DEGREES_TO_RADIANS(2.f));
            pos.normalize();
            pos.scale(offset->getPositionX());
            petalNode->setPosition(pos);
        }

        for (int i = 7; i < 15; i++) {
            auto petalNode = getChildByTag(i);
            auto pos = petalNode->getPosition();
            pos.rotate(VEC_ZERO, CC_DEGREES_TO_RADIANS(-3.f));
            pos.normalize();
            pos.scale(offset->getPositionY());
            petalNode->setPosition(pos);
        }
    }
    else {
        auto ship = neighbours->at(0);
        auto petalNode = ship->getChildByTag(shipID - 1);
        setPosition(ship->getPosition() + petalNode->getPosition());

        auto to_ship = ship->getPosition() - getPosition();
        setRotation(-CC_RADIANS_TO_DEGREES(to_ship.getAngle()) - 90);
    }
    Ship::update(delta);
}

cocos2d::Vec2 Blossom::cohesion()
{
    if (shipID == 0) return Vec2(0, 0);
    auto ship = neighbours->at(0);
    auto petalNode = ship->getChildByTag(shipID - 1);
    return seek(ship->getPosition() + petalNode->getPosition());
}

cocos2d::Vec2 Blossom::seekBits() {
    if (shipID == 0) return Ship::seekBits();
    return VEC_ZERO;
}

void Blossom::addPetalNodes()
{
    auto rot = CC_DEGREES_TO_RADIANS(360 / 7.f);
    auto pos_inner = Vec2(inner_radius_min, 0);
    auto pos_outer = Vec2(outer_radius_min, 0);
    for (int i = 0; i < 7; i++) {
        pos_inner.rotate(Vec2(0, 0), rot);
        auto node = Node::create();
        node->setPosition(pos_inner);
        addChild(node, 0, i);
    }

    for (int i = 7; i < 15; i++) {
        pos_outer.rotate(Vec2(0, 0), rot);
        auto node = Node::create();
        node->setPosition(pos_outer);
        addChild(node, 0, i);
    }

    // Offset node
    auto offset = Node::create();
    offset->setPosition(inner_radius_min, outer_radius_min);
    offset->runAction(RepeatForever::create(
        Sequence::create(
            EaseSineInOut::create(MoveTo::create(2, Vec2(inner_radius_max, outer_radius_max))),
            DelayTime::create(2),
            EaseSineInOut::create(MoveTo::create(2, Vec2(inner_radius_min, outer_radius_min))),
            nullptr
        )
    ));
    addChild(offset, 0, "offset");
}
