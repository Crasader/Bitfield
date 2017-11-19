#include "Sweeper.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "GameObject\Bit.h"
#include "UI\World.h"
USING_NS_CC;

Sweeper* Sweeper::create(World* world, SquadronInfo& info, int squadronID, int shipID) {
    Sweeper* ship = new (std::nothrow) Sweeper();
    if (ship && ship->init(world, info, squadronID, shipID)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Sweeper::update(float delta)
{
    auto cell = world->getCellIndex(getPosition());
    if (target_x == -1 || abs(cell.x - target_x) > 2) {
        target_x = cell.x;
        target_y = cell.y;
        getNextTarget();
    }

    setScale(1.0f - 0.15f * (velocity.length() / max_speed), 1.0f + 0.2f * (velocity.length() / max_speed));

    Ship::update(delta);
}

cocos2d::Color3B Sweeper::getStreakColor()
{
    return Color3B(Player::generators[BitType::Violet].color);
}

void Sweeper::loadInfo(World * world, SquadronInfo & info, int squadronID, int shipID)
{
    Ship::loadInfo(world, info, squadronID, shipID);
}

void Sweeper::calculateForces(float delta)
{
    auto target_pos = world->getCellPosition(target_x, target_y, true);
    auto dist = (target_pos - getPosition()).length();

    if (dist < 32 && velocity.lengthSquared() < 1) {
        world->addTileGlow(target_x, target_y, Color3B(Player::generators[BitType::Violet].color), 0.8f);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (!world->gridContains(target_x - 1 + i, target_y - 1 + j)) continue;
                world->addTileGlow(target_x - 1 + i, target_y - 1 + j, Color3B(Player::generators[BitType::Violet].color), 0.1f);
            }
        }
        world->consumeTile(target_x, target_y);
        getNextTarget();
    }

    applyForce(seek(target_pos, true));
}

void Sweeper::onBitPickup()
{
    // Do nothing
}

void Sweeper::getNextTarget()
{
    int old_x = target_x;
    int old_y = target_y;
    int dir = cocos2d::random() % 4; // l, r, d, u
    int max = 0;

    // Find best direction to go in
    if (world->gridContains(target_x - 1, target_y)) {
        auto bits = world->getBits(target_x - 1, target_y);
        if (bits.size() > max) {
            max = bits.size();
            dir = 0;
        }
        else if (bits.size() > 0 && bits.size() == max) {
            if (rand_0_1() > 0.5f) dir = 0;
        }
    }

    if (world->gridContains(target_x + 1, target_y)) {
        auto bits = world->getBits(target_x + 1, target_y);
        if (bits.size() > max) {
            max = bits.size();
            dir = 1;
        }
        else if (bits.size() > 0 && bits.size() == max) {
            if (rand_0_1() > 0.5f) dir = 1;
        }
    }

    if (world->gridContains(target_x, target_y - 1)) {
        auto bits = world->getBits(target_x, target_y - 1);
        if (bits.size() > max) {
            max = bits.size();
            dir = 2;
        }
        else if (bits.size() > 0 && bits.size() == max) {
            if (rand_0_1() > 0.5f) dir = 2;
        }

    }

    if (world->gridContains(target_x, target_y + 1)) {
        auto bits = world->getBits(target_x, target_y + 1);
        if (bits.size() > max) {
            dir = 3;
        }
        else if (bits.size() > 0 && bits.size() == max) {
            if (rand_0_1() > 0.5f) dir = 3;
        }
    }

    switch (dir) {
    case 0: target_x -= 1; break;
    case 1: target_x += 1; break;
    case 2: target_y -= 1; break;
    case 3: target_y += 1; break;
    }

    if (!world->gridContains(target_x, target_y)) {
        target_x = old_x;
        target_y = old_y;
    }
}