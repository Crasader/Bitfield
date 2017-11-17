#include "Barrier.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "UI/World.h"
USING_NS_CC;

Barrier::Barrier(World* world, SquadronInfo info, int squadronID, int shipID)
    : Ship(world, info, squadronID, shipID)
{
    start_scale = info.doubles["barrier_scale"];
    shrink_scale = info.doubles["shrink_scale"];
    grow_scale = info.doubles["grow_scale"];
    max_scale = info.doubles["max_scale"];
    
    if (shipID == 1) {
        point_to_velocity = false;
        setOpacity(255 * 0.15f);
        w_wander = 0;
        w_seek_bits = 0;
        scale = start_scale;
        stopAllActions();
        runAction(EaseElasticOut::create(ScaleTo::create(1.5f, start_scale)));
    }

    target_scale = scale;
}

Barrier* Barrier::create(World* world, SquadronInfo info, int squadronID, int shipID) {
    Barrier* ship = new (std::nothrow) Barrier(world, info, squadronID, shipID);
    
    std::string path;
    if (shipID == 0) {
        path = info.strings["sprite"];
    }
    else {
        path = info.strings["sprite_barrier"];
    }
    if (ship && ship->initWithFile(path)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Barrier::update(float delta)
{
    if (shipID == 1) {
        auto ratio = scale / max_scale * 1.2f;
        setPosition(world->getSquadron(squadronID).at(0)->getPosition());
        setRotationSkewX(getRotationSkewX() + 4 * ratio);
        setRotationSkewY(getRotationSkewY() + 4 * ratio);

        if (getNumberOfRunningActions() == 0) {
            target_scale -= shrink_scale * ratio;
            if (target_scale < start_scale) {
                target_scale = start_scale;
            }
            auto diff = target_scale - scale;
            scale += diff * 0.25f;
            setScale(scale);
        }
    }
    Ship::update(delta);
}

void Barrier::onBitPickup()
{
    if (shipID == 0) return;

    target_scale += grow_scale;
    if (target_scale > max_scale) {
        target_scale = max_scale;
    }
}