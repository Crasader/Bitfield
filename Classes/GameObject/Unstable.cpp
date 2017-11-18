#include "Unstable.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "GameObject\Bit.h"
#include "UI\World.h"
USING_NS_CC;

Unstable* Unstable::create(World* world, SquadronInfo& info, int squadronID, int shipID) {
    Unstable* ship = new (std::nothrow) Unstable();
    if (ship && ship->init(world, info, squadronID, shipID)) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

void Unstable::loadInfo(World * world, SquadronInfo& info, int squadronID, int shipID)
{
    Ship::loadInfo(world, info, squadronID, shipID);

    flames = 0;
    timer = 0;
    flame_t = info.doubles["flame_t"];
}

void Unstable::update(float delta)
{
    timer += delta;
    if (flames == 0 && timer > flame_t) {
        igniteRandomTile();
    }
    else if (flames == 1 && timer > flame_t + 0.25f) {
        igniteRandomTile();
    }
    else if (flames == 2 && timer > flame_t + 0.5f) {
        igniteRandomTile();
        timer = 0;
        flames = 0;
    }
    Ship::update(delta);
}

cocos2d::Color3B Unstable::getStreakColor()
{
    return Color3B(UI_COLOR_RED);
}

void Unstable::igniteRandomTile() {
    auto cell = world->getCellIndex(getPosition());

    auto target_cell = cell;
    target_cell.x += -2 + cocos2d::random() % 5;
    target_cell.y += -2 + cocos2d::random() % 5;
    if (!world->gridContains(target_cell.x, target_cell.y)) return;

    world->addTileGlow(target_cell.x, target_cell.y, Color3B(UI_COLOR_RED), 0.6f);
    //for (int i = 0; i < 3; i++) {
    //    for (int j = 0; j < 3; j++) {
    //        if (!world->gridContains(target_cell.x - 1 + i, target_cell.y - 1 + j)) continue;
    //        world->addTileGlow(target_cell.x - 1 + i, target_cell.y - 1 + j, Color3B(UI_COLOR_RED), 0.1f);
    //    }
    //}
    world->consumeTile(target_cell.x, target_cell.y);
    flames++;
}