#include "Ship.h"

#include "Util.h"
#include "Scene\GameScene.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Bit.h"

USING_NS_CC;

Ship::Ship(SquadronInfo info)
{
    type = info.strings["type"];
    sprite = info.strings["sprite"];

    vision_radius = info.ints["vision_radius"];
    separation_radius = info.ints["separation_radius"];

    max_speed = info.doubles["max_speed"];
    max_force = info.doubles["max_force"];
    w_alignment = info.doubles["w_alignment"];
    w_cohesion = info.doubles["w_cohesion"];
    w_separation = info.doubles["w_separation"];
    w_wander = info.doubles["w_wander"];
    w_seek = info.doubles["w_seek"];
    w_seek_bits = info.doubles["w_seek_bits"];

    scheduleUpdate();
    velocity = Vec2(0.1f, 0.1f);
    acceleration = Vec2(0, 0);
    //velocity = velocity.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(rand_0_1() * 360));
}

void Ship::update(float delta) {
    Sprite::update(delta);
    
    // Forces --> Acceleration
    acceleration = Vec2(0, 0);
    calculateForces();

    // Movement
    velocity += acceleration; // We never "desire" to go faster than max_speed, so don't have to limit here
    setPosition(getPosition() + velocity);
    
    // Point towards velocity
    this->setRotation3D(Vec3(getRotation3D().x, getRotation3D().y, -CC_RADIANS_TO_DEGREES(velocity.getAngle()) + 90));
}

void Ship::calculateForces() {
    auto stayWithinForce = stayWithin(boundary);
    if (!stayWithinForce.isZero()) {
        applyForce(stayWithinForce, w_avoid_wall);
        return;
    }
    else if (Util::touch_down && w_seek > 0) {
        // Seek towards touch position
        auto target = Util::touch_location;
        Vec2 seekForce = seek(target);
        applyForce(seekForce, w_seek);
    }
    else {
        // Seek to nearby bit or wander
        Vec2 bitForce = seekBits(*bits);
        if (!bitForce.isZero() && w_seek_bits > 0) {
            applyForce(bitForce, w_seek_bits);
        }
        else {
            Vec2 wanderForce = wander();
            applyForce(wanderForce, w_wander);
        }
    
        // Flock
        Vec2 alignForce = align(*neighbours);
        applyForce(alignForce, w_alignment);
        Vec2 cohesionForce = cohesion(*neighbours);
        applyForce(cohesionForce, w_cohesion);
        Vec2 separateForce = separate(*neighbours);
        applyForce(separateForce, w_separation);
    }
}

void Ship::applyForce(cocos2d::Vec2 force, float scale) {
    force.scale(scale);
    acceleration.add(force);
}

cocos2d::Vec2 Ship::wander() {
    // Project future position
    Vec2 projection(velocity);
    projection.normalize();
    projection.scale(wander_length);

    // Point to a random spot on the circle
    Vec2 toRadius = Vec2(wander_radius, 0);
    wander_theta += rand_minus1_1() * wander_delta;
    toRadius = toRadius.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(wander_theta));

    return seek(getPosition() + projection + toRadius);
}

// Steer away from nearby ships
cocos2d::Vec2 Ship::separate(const cocos2d::Vector<Ship*>& neighbours) {
    Vec2 desired(0, 0);
    float count = 0;

    // Steer away from nearby ships
    for (Ship* ship : neighbours) {
        Vec2 toOther = ship->getPosition() - getPosition();
        if (toOther.getLength() > 0 && toOther.getLength() < separation_radius) {
            Vec2 awayFromOther = getPosition() - ship->getPosition();
            awayFromOther.normalize();
            awayFromOther.scale(1 / awayFromOther.getLength());
            desired.add(awayFromOther);
            count += 1;
        }
    }

    if (count > 0) {
        desired.scale(1.0f / count);
        desired.normalize();
        desired.scale(max_speed);

        Vec2 steer = desired - velocity;
        if (steer.length() > max_force) {
            steer.normalize();
            steer.scale(max_force);
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
        if (ship == this) continue;
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
    Vec2 desired(0, 0);
    float count = 0;

    // Average together every ship's heading and try to align with the group
    for (Ship* ship : neighbours) {
        if (!inRange(ship)) continue;
        auto dir = ship->getVelocity();
        desired.add(dir);
        count += 1;
    }

    if (count > 0) {
        desired.scale(1.0f / count);
        desired.normalize();
        desired.scale(max_speed);

        Vec2 steer = desired - velocity;
        if (steer.length() > max_force) {
            steer.normalize();
            steer.scale(max_force);
        }
        return steer;
    }

    return Vec2(0, 0);
}

// Steer towards the target location
cocos2d::Vec2 Ship::seek(cocos2d::Vec2 target, bool slowDown) {
    Vec2 desired = target - getPosition();
    float length = desired.length();
    desired.normalize();
    desired.scale(max_speed);

    Vec2 steer = desired - velocity;
    if (steer.length() > max_force) {
        steer.normalize();
        steer.scale(max_force);
    }
    return steer;
}

// Seek towards closest bit
cocos2d::Vec2 Ship::seekBits(std::map< BitType, cocos2d::Vector< Bit* > >& bits) {
    // Find the closest bit and seek to it
    Bit* nearestBit = getClosestBit();

    if (nearestBit != nullptr) {
        auto target = nearestBit->getPosition();
        return seek(target);
    }

    return Vec2(0, 0);
}

cocos2d::Vec2 Ship::stayWithin(cocos2d::Rect boundary) {
    auto heading = velocity.getNormalized();
    heading.scale(wall_separation_distance);
    if (boundary.containsPoint(getPosition() + heading)) return Vec2(0, 0);
    
    auto mid = Vec2(boundary.getMidX(), boundary.getMidY());
    auto toTarget = mid - getPosition();
    wander_theta = CC_RADIANS_TO_DEGREES(toTarget.getAngle());
    return seek(mid);
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
    return toTarget.length() <= vision_radius;
}

const cocos2d::Vec2& Ship::getVelocity() {
    return velocity;
}
const cocos2d::Vec2& Ship::getAcceleration() {
    return acceleration;
}

void Ship::setNeighbours(cocos2d::Vector<Ship*>* neighbours)
{
    this->neighbours = neighbours;
}

void Ship::setBits(std::map<BitType, cocos2d::Vector<Bit*>>* bits)
{
    this->bits = bits;
}

void Ship::setBoundary(cocos2d::Rect boundary)
{
    this->boundary = boundary;
}

Bit * Ship::getClosestBit()
{
    Bit* nearestBit = nullptr;
    for (auto map_pair : *bits) {
        auto vec = map_pair.second;
        for (Bit* bit : vec) {
            if (!inRange(bit)) continue;
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
    }
    return nearestBit;
}

