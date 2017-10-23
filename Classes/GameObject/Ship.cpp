#include "Ship.h"

#include "Util.h"
#include "Scene\GameScene.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Bit.h"

USING_NS_CC;

void Ship::onEnter() {
    Sprite::onEnter();

    velocity = Vec2(Player::ship_speed, 0);
    velocity = velocity.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(rand_0_1() * 360));

    targetOffset = Vec2(50, 0);
    targetOffset = targetOffset.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(rand_0_1() * 360));

    scheduleUpdate();
}

void Ship::onExit() {
    Sprite::onExit();
    unscheduleUpdate();
}

void Ship::update(float delta) {
    Sprite::update(delta);
    acceleration = Vec2(0, 0);

    calculateForces();

    // Movement
    velocity += acceleration;
    setPosition(getPosition() + velocity);
    if (velocity.length() > Player::ship_speed) {
        velocity.normalize();
        velocity.scale(Player::ship_speed);
    }
    
    // Point towards velocity
    this->setRotation3D(Vec3(getRotation3D().x, getRotation3D().y, -CC_RADIANS_TO_DEGREES(velocity.getAngle()) + 90));
}

void Ship::calculateForces() {
    if (Util::touch_down /*&& ship == ships.at(0)*/) {
        // Seek towards touch position
        auto target = Util::touch_location - getPosition();
        Vec2 seekForce = seek(target);
        applyForce(seekForce, Player::seek);
        auto toMouse = target - getPosition();
        toMouse.normalize();
        toMouse.scale(50);
    }
    else {
        // Seek to the closest, largest value bit
        Vec2 bitForce;
        for (int i = BitType::All - 1; i >= 0; i--) {
            bitForce = seekBits((*bits)[BitType(i)]);
            if (!bitForce.isZero()) {
                break;
            }
        }
        if (!bitForce.isZero()) {
            applyForce(bitForce, Player::seekBits);
        }
        else {
            Vec2 wanderForce = wander();
            applyForce(wanderForce, Player::wander);
        }

        // Flock
        Vec2 alignForce = align(*neighbours);
        applyForce(alignForce, Player::alignment);
        Vec2 cohesionForce = cohesion(*neighbours);
        applyForce(cohesionForce, Player::cohesion);
        Vec2 separateForce = separate(*neighbours);
        applyForce(separateForce, Player::separation);
    }
}

void Ship::applyForce(cocos2d::Vec2 force, float scale) {
    force.scale(scale);
    acceleration.add(force);
}

cocos2d::Vec2 Ship::wander() {
    Vec2 toFuturePos = velocity;
    toFuturePos.normalize();
    toFuturePos.scale(300);

    // Target a random future position
    Vec2 futurePos = getPosition() + toFuturePos;
    float randomAngle = CC_DEGREES_TO_RADIANS(rand_minus1_1() * 7);
    targetOffset = targetOffset.rotateByAngle(Vec2(0, 0), randomAngle);
    futurePos += targetOffset;

    return seek(futurePos);
}

// Steer towards the target location
cocos2d::Vec2 Ship::seek(cocos2d::Vec2 target, bool slowDown) {
    Vec2 desired = target - getPosition();
    float length = desired.length();
    desired.normalize();

    // If we are slowing down, scale speed relative to how close we are to the target
    if (slowDown && length < Player::ship_vision) {
        desired.scale(length / Player::ship_vision * Player::ship_speed);
    }
    else {
        desired.scale(Player::ship_speed);
    }

    Vec2 steer = desired - velocity;
    if (steer.length() > Player::ship_force) {
        steer.normalize();
        steer.scale(Player::ship_force);
    }
    return steer;
}

// Steer away from other ships
cocos2d::Vec2 Ship::separate(const cocos2d::Vector<Ship*>& neighbours) {
    Vec2 desired;
    float count = 0;

    // Steer away from nearby ships
    for (Ship* ship : neighbours) {
        Vec2 toOther = ship->getPosition() - getPosition();
        if (toOther.getLength() > 0 && toOther.getLength() < Player::ship_separation) {
            Vec2 awayFromOther = getPosition() - ship->getPosition();
            awayFromOther.normalize();
            awayFromOther.scale(Player::ship_separation / awayFromOther.getLength());
            desired.add(awayFromOther);
            count += 1;
        }
    }

    if (count > 0) {
        desired.scale(1.0f / count);
        if (desired.length() > Player::ship_speed) {
            desired.normalize();
            desired.scale(Player::ship_speed);
        }

        Vec2 steer = desired - velocity;
        if (steer.length() > Player::ship_force) {
            steer.normalize();
            steer.scale(Player::ship_force);
        }
        return steer;
    }

    return Vec2(0, 0);
}

// Steer towards center of mass
cocos2d::Vec2 Ship::cohesion(const cocos2d::Vector<Ship*>& neighbours) {
    Vec2 desired;
    float count = 0;

    // Average together every ship's position to (roughly) find the center of mass
    for (Ship* ship : neighbours) {
        if (!canSee(ship)) continue;
        desired.add(ship->getPosition());
        count += 1;
    }

    if (count > 0) {
        desired.scale(1.0f / count);
        return seek(desired);
    }

    return Vec2(0, 0);
}

// Align direction with nearby ships
cocos2d::Vec2 Ship::align(const cocos2d::Vector<Ship*>& neighbours) {
    Vec2 desired;
    float count = 0;

    // Average together every ship's heading and try to align with the group
    for (Ship* ship : neighbours) {
        if (!canSee(ship)) continue;
        //ship->setColor(Color3B::ORANGE);
        auto dir = ship->getVelocity();
        dir.normalize();
        desired.add(dir);
        count += 1;
    }

    if (count > 0) {
        desired.scale(1.0f / count);
        desired.normalize();
        desired.scale(Player::ship_speed);

        Vec2 steer = desired - velocity;
        if (steer.length() > Player::ship_force) {
            steer.normalize();
            steer.scale(Player::ship_force);
        }
        return steer;
    }

    return Vec2(0, 0);
}

// Seek towards closest bit
cocos2d::Vec2 Ship::seekBits(const cocos2d::Vector<Bit*>& neighbours) {
    // Average together every ship's heading and try to align with the group
    Bit* nearestBit = nullptr;
    for (Bit* bit : neighbours) {
        if (!canSee(bit)) continue;
        if (nearestBit == nullptr) {
            nearestBit = bit;
            continue;
        }
        auto toBit = bit->getPosition() - getPosition();
        auto toNearest = nearestBit->getPosition() - getPosition();
        if (toBit.lengthSquared() < toNearest.lengthSquared()) {
            nearestBit = bit;
        }
    }
    if (nearestBit != nullptr) {
        auto target = nearestBit->getPosition();
        return seek(target);
    }

    return Vec2(0, 0);
}

bool Ship::isFront(const cocos2d::Vector<Ship*>& neighbours) {
    for (Ship* ship : neighbours) {
        if (canSee(ship)) return false;
    }
    return true;
}

bool Ship::canSee(cocos2d::Node* target) {
    if (target == this || !inRange(target)) return false;

    auto heading = getVelocity().getNormalized();
    auto toTarget = (target->getPosition() - getPosition()).getNormalized();
    auto angle = CC_RADIANS_TO_DEGREES(Vec2::angle(heading, toTarget));
    return (angle >= 0 && angle <= 135);
}

bool Ship::inRange(cocos2d::Node* target) {
    if (target == this) return false;
    auto toTarget = target->getPosition() - getPosition();
    return toTarget.length() <= Player::ship_vision;
}

const cocos2d::Vec2& Ship::getVelocity() {
    return velocity;
}
const cocos2d::Vec2& Ship::getAcceleration() {
    return acceleration;
}

const cocos2d::Vec2& Ship::getTargetOffset() {
    return targetOffset;
}

void Ship::setNeighbours(cocos2d::Vector<Ship*>* neighbours)
{
    this->neighbours = neighbours;
}

void Ship::setBits(std::map<BitType, cocos2d::Vector<Bit*>>* bits)
{
    this->bits = bits;
}
