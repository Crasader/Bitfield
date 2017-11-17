#include "Ship.h"

#include "Util.h"
#include "Input.h"
#include "Scene\GameScene.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Bit.h"
#include "UI\World.h"

USING_NS_CC;

Ship::Ship(World* world, SquadronInfo info, int squadronID, int shipID)
{
    type = info.strings["type"];
    sprite = info.strings["sprite"];

    count = info.ints["count"];
    vision_radius = info.ints["vision_radius"];
    separation_radius = info.ints["separation_radius"];
    wall_separation_distance = info.ints["wall_separation_distance"];
    wander_length = info.ints["wander_length"];
    wander_radius = info.ints["wander_radius"];
    wander_theta = 0;
    wander_delta = info.ints["wander_delta"];

    scale = info.doubles["scale"];
    max_speed = info.doubles["max_speed"];
    max_force = info.doubles["max_force"];
    w_alignment = info.doubles["w_alignment"];
    w_cohesion = info.doubles["w_cohesion"];
    w_separation = info.doubles["w_separation"];
    w_wander = info.doubles["w_wander"];
    w_seek = info.doubles["w_seek"];
    w_seek_bits = info.doubles["w_seek_bits"];
    w_avoid_wall = info.doubles["w_avoid_wall"];
    w_leash = info.doubles["w_leash"];

    scheduleUpdate();
    velocity = Vec2(0, 0);
    acceleration = Vec2(0, 0);
    point_to_velocity = true;
    this->squadronID = squadronID;
    this->shipID = shipID;
    this->world = world;

    setScale(0);
    runAction(EaseElasticOut::create(ScaleTo::create(1.5f, scale)));
}

Ship* Ship::create(World* world, SquadronInfo info, int squadronID, int shipID) {
    Ship* ship = new (std::nothrow) Ship(world, info, squadronID, shipID);
    if (ship && ship->initWithFile(ship->getSprite())) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Ship::update(float delta) {
    Sprite::update(delta);

    // Forces --> Acceleration
    acceleration = Vec2(0, 0);
    calculateForces(delta);

    // Movement
    velocity += acceleration; // We never "desire" to go faster than max_speed, so don't have to limit here
    auto futurePos = getPosition() + velocity;
    Util::capVector(futurePos, Rect(0, 0, WORLD_WIDTH, WORLD_HEIGHT));
    setPosition(futurePos);
    handleCollisions();

    // Point towards velocity
    if (point_to_velocity)
        setRotation3D(Vec3(getRotation3D().x, getRotation3D().y,
            -CC_RADIANS_TO_DEGREES(velocity.getAngle()) + 90));
}

void Ship::calculateForces(float delta) {
    // Always avoid walls and other ships
    auto f_avoid_walls = avoidWalls();
    if (!f_avoid_walls.isZero()) {
        applyForce(f_avoid_walls, w_avoid_wall);
        return;
    }
    Vec2 separateForce = separate();
    applyForce(separateForce, w_separation);

    // Seek towards touch position
    if (shipID == 0 && squadronID == Player::slot_selected && Input::touch_down) {
        auto target = Input::touch_pos - getParent()->getPosition();
        Util::capVector(target, Rect(0, 0, WORLD_WIDTH, WORLD_HEIGHT));
        Vec2 seekForce = seek(target);
        applyForce(seekForce, w_seek);
    }
    else {
        // Stay with leader if too far
        if (shipID > 0 && w_leash > 0) {
            auto squadron = world->getSquadron(squadronID);
            auto leader = squadron.at(0);
            if (getPosition().distance(leader->getPosition()) > vision_radius) {
                auto stayGroupedForce = leash();
                auto toLeader = leader->getPosition() -getPosition();
                wander_theta = CC_RADIANS_TO_DEGREES(toLeader.getAngle());
                applyForce(stayGroupedForce, w_leash);
                return;
            }
        }

        // Seek bits
        Vec2 bitForce = seekBits();
        if (!bitForce.isZero()) {
            applyForce(bitForce, w_seek_bits);
        }
        else {
            // Wander
            Vec2 wanderForce = wander();
            applyForce(wanderForce, w_wander);

            // Flock
            Vec2 alignForce = align();
            applyForce(alignForce, w_alignment);
            Vec2 cohesionForce = cohesion();
            applyForce(cohesionForce, w_cohesion);
        }
    }
}

void Ship::handleCollisions()
{
    auto pos = getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;

    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            if (!world->gridContains(r, c)) continue;

            for (auto bit : world->getBits(r, c)) {
                Vec2 dist = bit->getPosition() - getPosition();
                if (dist.getLength() < getContentSize().height * scale / 2.f) {
                    if (bit->isRemoved()) continue;
                    auto& info = Player::generators[bit->getType()];
                    auto value = Player::calculateValue(bit->getType());
                    Player::addBits(value);
                    info.spawned--;

                    // Remove bit
                    Player::dispatchEvent(EVENT_BIT_PICKUP, (void*)bit);
                    bit->remove();
                    onBitPickup();
                }
            }
        }
    }
}

void Ship::applyForce(cocos2d::Vec2 force, float scale) {
    force.scale(scale);
    acceleration.add(force);
}

cocos2d::Vec2 Ship::wander() {
    if (w_wander == 0) return VEC_ZERO;

    // Project future position
    Vec2 projection(velocity);
    projection.normalize();
    projection.scale(wander_length);

    // Point to a random spot on the circle
    Vec2 toRadius(wander_radius, 0);
    wander_theta = (int)(wander_theta + cocos2d::rand_minus1_1() * wander_delta) % 360;
    toRadius = toRadius.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(wander_theta));

    return seek(getPosition() + projection + toRadius);
}

// Align direction with nearby ships
cocos2d::Vec2 Ship::align() {
    if (w_alignment == 0) return VEC_ZERO;

    Vec2 desired(0, 0);
    float count = 0;

    // Average together every ship's heading and try to align with the group
    for (Ship* ship : world->getSquadron(squadronID)) {
        if (!canSee(ship)) continue;
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

// Steer towards the center of mass
cocos2d::Vec2 Ship::cohesion() {
    if (w_cohesion == 0) return VEC_ZERO;

    auto center = getCenterOfSquadron();
    if (world->getSquadron(squadronID).empty() || center == getPosition()) return VEC_ZERO;

    return seek(center, true);
}

// Steer away from nearby ships
cocos2d::Vec2 Ship::separate() {
    if (w_separation == 0) return VEC_ZERO;

    Vec2 desired(0, 0);
    float count = 0;

    // Steer away from nearby ships
    for (Ship* ship : world->getSquadron(squadronID)) {
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

// Steer towards the target location
cocos2d::Vec2 Ship::seek(cocos2d::Vec2 target, bool slowdown) {
    Vec2 desired = target - getPosition();
    float dist = desired.length();
    desired.normalize();

    auto scl = 0;
    if (slowdown && dist <= separation_radius) {
        scl = max_speed * (dist / separation_radius);
    }
    else {
        scl = max_speed;
    }
    desired.scale(scl);

    Vec2 steer = desired - velocity;
    if (steer.length() > max_force) {
        steer.normalize();
        steer.scale(max_force);
    }
    return steer;
}

// Seek towards closest bit
cocos2d::Vec2 Ship::seekBits() {
    if (w_seek_bits == 0) return VEC_ZERO;

    auto targetBit = getTargetBit();
    if (targetBit) {
        return seek(targetBit->getPosition());
    }
    
    return VEC_ZERO;
}

// Steer away from walls
cocos2d::Vec2 Ship::avoidWalls() {
    if (w_avoid_wall == 0) return VEC_ZERO;

    auto heading = velocity.getNormalized();
    heading.scale(wall_separation_distance);
    auto boundary = Rect(VEC_ZERO, world->getContentSize());
    if (boundary.containsPoint(getPosition() + heading)) return VEC_ZERO;

    auto mid = Vec2(boundary.getMidX(), boundary.getMidY());
    auto toTarget = mid - getPosition();
    wander_theta = CC_RADIANS_TO_DEGREES(toTarget.getAngle());
    return seek(mid);
}

cocos2d::Vec2 Ship::leash() {
    if (shipID == 0) return VEC_ZERO;
    return seek(world->getSquadron(squadronID).at(0)->getPosition());
}

bool Ship::canSee(cocos2d::Node* target) {
    if (target == this || !inRange(target)) return false;

    auto heading = getVelocity().getNormalized();
    auto toTarget = (target->getPosition() - getPosition()).getNormalized();
    auto angle = CC_RADIANS_TO_DEGREES(Vec2::angle(heading, toTarget));
    return angle >= 0 && angle <= 90;
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

// Target the closest untargetted bit. We may already be targetting it.
// TODO: This logic needs some work:
// What if a ship targets a bit and then moves out of grid range?
Bit* Ship::getTargetBit()
{
    Bit* previousTarget = nullptr;
    Bit* nearestBit = nullptr;
    auto toNearest = Vec2(0, 0);

    auto pos = getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;

    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            if (!world->gridContains(r, c)) continue;
            auto& bits = world->getBits(r, c);
            for (auto bit : bits) {
                // Clear out of range targets
                if (bit->isTargettedBy(this) && !canSee(bit)) bit->setShip(nullptr);
                            
                // If Bit is targetted by another ship, ignore it
                if (bit->isTargetted() && !bit->isTargettedBy(this) || !canSee(bit)) continue;

                // Check if bit is our last target
                if (bit->isTargettedBy(this)) {
                    previousTarget = bit;
                }

                // Found initial bit
                if (nearestBit == nullptr) {
                    nearestBit = bit;
                    toNearest = nearestBit->getPosition() - getPosition();
                }
                else {
                    // See if new bit is closer than current closest
                    auto toBit = bit->getPosition() - getPosition();
                    if (toBit.lengthSquared() < toNearest.lengthSquared()) {
                        nearestBit->setShip(nullptr);
                        nearestBit = bit;
                    }
                }
            }
        }
    }

    // Found a nearby bit that isn't targetted by another ship
    if (nearestBit) {
        // If it's not what we were targetting before, clear old and set up new
        if (previousTarget && previousTarget != nearestBit) {
            previousTarget->setShip(nullptr);
        }
        nearestBit->setShip(this);
    }

    return nearestBit;
}

const std::string& Ship::getType()
{
    return type;
}

const std::string & Ship::getSprite()
{
    return sprite;
}

cocos2d::Vec2 Ship::getCenterOfSquadron()
{
    Vec2 center;
    for (Ship* ship : world->getSquadron(squadronID)) {
        center.add(ship->getPosition());
    }

    center.scale(1.0f / world->getSquadron(squadronID).size());
    return center;
}

void Ship::onBitPickup()
{
    runAction(Sequence::create(
        ScaleTo::create(0.1f, scale * 1.25f),
        ScaleTo::create(0.1f, scale),
        nullptr
    ));
}