#include "Ship.h"

#include "Util.h"
#include "Scene\GameScene.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Bit.h"

USING_NS_CC;

Ship::Ship(SquadronInfo info, int squadronID, int shipID)
{
    type = info.strings["type"];
    sprite = info.strings["sprite"];

    vision_radius = info.ints["vision_radius"];
    separation_radius = info.ints["separation_radius"];
    wall_separation_distance = info.ints["wall_separation_distance"];
    wander_length = info.ints["wander_length"];
    wander_radius = info.ints["wander_radius"];
    wander_theta = 0;
    wander_delta = info.ints["wander_delta"];

    max_speed = info.doubles["max_speed"];
    max_force = info.doubles["max_force"];
    w_alignment = info.doubles["w_alignment"];
    w_cohesion = info.doubles["w_cohesion"];
    w_separation = info.doubles["w_separation"];
    w_wander = info.doubles["w_wander"];
    w_seek = info.doubles["w_seek"];
    w_seek_bits = info.doubles["w_seek_bits"];
    w_avoid_wall = info.doubles["w_avoid_wall"];
    w_stay_grouped = info.doubles["w_stay_grouped"];

    scheduleUpdate();
    velocity = Vec2(0, 0);
    acceleration = Vec2(0, 0);
    this->squadronID = squadronID;
    this->shipID = shipID;
}

Ship* Ship::create(SquadronInfo info, int squadronID, int shipID) {
    Ship* ship = new (std::nothrow) Ship(info, squadronID, shipID);
    if (ship && ship->initWithFile(info.strings["sprite"])) {
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
    setPosition(getPosition() + velocity);
    handleCollisions();

    // Point towards velocity
    setRotation3D(Vec3(getRotation3D().x, getRotation3D().y, -CC_RADIANS_TO_DEGREES(velocity.getAngle()) + 90));
}

void Ship::calculateForces(float delta) {
    // Stay in World Boundary
    auto stayWithinForce = stayWithin();
    if (!stayWithinForce.isZero()) {
        applyForce(stayWithinForce, w_avoid_wall);
        return;
    }

    // Seek towards touch position
    if (shipID == 0 && squadronID == 0 && Util::touch_down) {
        Vec2 seekForce = seek(Util::touch_location);
        applyForce(seekForce, w_seek);
        return;
    }

    // Stay with group
    auto stayGroupedForce = stayGrouped();
    if (!stayGroupedForce.isZero() && w_stay_grouped > 0) {
        auto center = getCenterOfSquadron(*neighbours);
        auto toCenter = center - getPosition();
        wander_theta = CC_RADIANS_TO_DEGREES(toCenter.getAngle());
        applyForce(stayGroupedForce, w_stay_grouped);
        return;
    }

    // Seek bits
    Vec2 bitForce = seekBits();
    if (!bitForce.isZero() && w_seek_bits > 0) {
        applyForce(bitForce, w_seek_bits);
    }
    else {
        Vec2 wanderForce = wander();
        applyForce(wanderForce, w_wander);

        // Flock
        if (shipID != 0) {
            Vec2 alignForce = align();
            applyForce(alignForce, w_alignment);
            Vec2 cohesionForce = cohesion();
            applyForce(cohesionForce, w_cohesion);
            Vec2 separateForce = separate();
            applyForce(separateForce, w_separation);
        }
    }
}

void Ship::handleCollisions()
{
    auto pos = getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;

    for (int r = row - 1; r < row + 1; r++) {
        for (int c = col - 1; c < col + 1; c++) {
            if (r >= 0 && c >= 0 && r < GRID_WIDTH && c < GRID_HEIGHT) {
                auto& grid = (*bits);
                for (auto bit : grid[r][c]) {
                    Vec2 dist = bit->getPosition() - getPosition();
                    if (dist.getLength() < getContentSize().width / 2) {
                        if (bit->isRemoved()) continue;
                        auto& info = Player::bit_info[bit->getType()];
                        auto value = Player::calculateValue(bit->getType());
                        Player::addBits(value);
                        info.spawned--;
                        addValuePopup(bit);

                        // Remove bit
                        bit->remove();
                    }
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
    Vec2 desired(0, 0);
    float count = 0;

    // Average together every ship's heading and try to align with the group
    for (Ship* ship : *neighbours) {
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

// Steer towards leader
cocos2d::Vec2 Ship::cohesion() {
    if (this == (*neighbours).at(0)) return Vec2(0, 0);
    return seek((*neighbours).at(0)->getPosition());
}

// Steer away from nearby ships
cocos2d::Vec2 Ship::separate() {
    Vec2 desired(0, 0);
    float count = 0;

    // Steer away from nearby ships
    for (Ship* ship : *neighbours) {
        //if (!canSee(ship)) continue;
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
cocos2d::Vec2 Ship::seekBits() {
    auto targetBit = getTargetBit();
    if (targetBit) {
        return seek(targetBit->getPosition());
    }
    
    return Vec2(0, 0);
}

cocos2d::Vec2 Ship::stayWithin() {
    auto heading = velocity.getNormalized();
    heading.scale(wall_separation_distance);
    if (boundary.containsPoint(getPosition() + heading)) return Vec2(0, 0);

    auto mid = Vec2(boundary.getMidX(), boundary.getMidY());
    auto toTarget = mid - getPosition();
    wander_theta = CC_RADIANS_TO_DEGREES(toTarget.getAngle());
    return seek(mid);
}

cocos2d::Vec2 Ship::stayGrouped() {
    auto center = getCenterOfSquadron(*neighbours);
    if (center.isZero() || center.getDistance(getPosition()) < vision_radius || shipID == 0) return Vec2(0, 0);
    return seek(center);
}

bool Ship::canSee(cocos2d::Node* target) {
    if (target == this || !inRange(target)) return false;

    auto heading = getVelocity().getNormalized();
    auto toTarget = (target->getPosition() - getPosition()).getNormalized();
    auto angle = CC_RADIANS_TO_DEGREES(Vec2::angle(heading, toTarget));
    return (angle >= 0 && angle <= 180);
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

void Ship::setBits(Grid* bits)
{
    this->bits = bits;
}

void Ship::setBoundary(cocos2d::Rect boundary)
{
    this->boundary = boundary;
}

// Target the closest untargetted bit. We may already be targetting it.
Bit* Ship::getTargetBit()
{
    Bit* previousTarget = nullptr;
    Bit* nearestBit = nullptr;
    auto toNearest = Vec2(0, 0);

    auto pos = getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;

    for (int r = row - 1; r < row + 1; r++) {
        for (int c = col - 1; c < col + 1; c++) {
            if (r >= 0 && c >= 0 && r < GRID_WIDTH && c < GRID_HEIGHT) {
                auto& grid = (*bits);
                for (auto bit : grid[r][c]) {
                    // Clear out of range targets
                    if (bit->isTargettedBy(this) && !canSee(bit)) {
                        bit->setShip(nullptr);
                    }
                            
                    // If Bit is targetted by another ship or out of range or removed, ignore it
                    if ((bit->isTargetted() && !bit->isTargettedBy(this)) || !canSee(bit)) continue;

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
                            nearestBit = bit;
                        }
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

cocos2d::Vec2 Ship::getCenterOfSquadron(const cocos2d::Vector<Ship*>& neighbours)
{
    Vec2 center;
    int count = 0;

    // Average together every ship's position to (roughly) find the center of mass
    for (Ship* ship : neighbours) {
        if (ship == this) continue;
        center.add(ship->getPosition());
        count += 1;
    }

    if (count > 0) {
        center.scale(1.0f / count);
        return center;
    }

    return Vec2(0, 0);
}

void Ship::addValuePopup(Bit* bit)
{
    auto world = getParent();

    //auto popup = Util::createIconLabel(0, Player::calculateValue(bit->getType()), 40);
    auto popup = Label::createWithTTF(Player::bit_info[bit->getType()].valueString, FONT_DEFAULT, 52);
    auto c = Color3B(Player::bit_info[bit->getType()].color);
    popup->setColor(Color3B(c.r * 1.25f, c.g * 1.25f, c.b * 1.25f));
    popup->setPosition(bit->getPosition());
    popup->setOpacity(0);
    popup->runAction(Sequence::create(
        Spawn::createWithTwoActions(
            EaseInOut::create(FadeIn::create(0.1f), 2),
            EaseInOut::create(ScaleTo::create(0.1f, 1.3f), 2)
        ),
        DelayTime::create(0.5f),
        Spawn::createWithTwoActions(
            EaseInOut::create(FadeOut::create(0.3f), 2),
            EaseInOut::create(ScaleTo::create(0.3f, 0.5f), 2)
        ),
        DelayTime::create(0.3f),
        RemoveSelf::create(),
        nullptr)
    );
    world->addChild(popup, 5);
}

