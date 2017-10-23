#include "World.h"

#include "Util.h"
#include "..\Constants.h"
#include "..\Scene\GameScene.h"
#include "..\GameObject\Wanderer.h"
#include "..\GameObject\Bit.h"
#include "ui\UIText.h"

USING_NS_CC;

World* World::create() {
    World *world = new (std::nothrow) World();
    if (world && world->init()) {
        world->autorelease();
        return world;
    } else {
        CC_SAFE_DELETE(world);
        return nullptr;
    }
}

void World::onEnter() {
    Layer::onEnter();

    scheduleUpdate();
    setCascadeOpacityEnabled(true);

    createGrid();
    createInput();

    // Spawn Ships
    addShip();
    auto ship = ships.at(0);
    ship->setPosition(getContentSize() / 2.0f);
    auto follow = Follow::createWithOffset(ship, 0, 350, Rect(0, -730, getContentSize().width, getContentSize().height + 730));
    runAction(follow);
}

void World::onExit() {
    Layer::onExit();
    unscheduleUpdate();
}

void World::update(float delta) {
    Layer::update(delta);

    updateShips();  
    handleSpawns(delta);
    handleCollisions();
}

cocos2d::Vector<Ship*>& World::getShips() {
    return ships;
}

void World::addShip() {
    auto ship = Wanderer::create();
    ship->setNeighbours(&ships);
    ship->setBits(&bits);
    ship->initWithFile(SPRITE_SHIP);
    if (ships.size() > 0) {
        ship->setPosition(ships.at(0)->getPosition());
    }
    addChild(ship, 3);
    ships.pushBack(ship);
}

void World::removeShip() {
    if (ships.size() < 1) return;
    auto ship = ships.back();
    ship->removeFromParentAndCleanup(true);
    ships.popBack();
}

void World::addBit(BitType type) {
    auto bit = Bit::create(type);
    bit->setPosition(getContentSize().width * (0.05f + rand_0_1() * 0.9f),
        getContentSize().height * (0.05f + rand_0_1() * 0.9f));
    addChild(bit, 2);
    bits[type].pushBack(bit);
}

void World::removeBit(BitType type) {
    if (bits.size() < 1) return;
    auto bit = bits[type].back();
    bit->removeFromParentAndCleanup(true);
    bits[type].popBack();
}

void World::createGrid() {
    // Create Background
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2(0, -730), Size(WORLD_WIDTH, WORLD_HEIGHT + 730), Color4F(WORLD_COLOR));
    addChild(drawNode, 0);

    // Create grid
    drawNode->setLineWidth(1);
    const int RES = GRID_RESOLUTION;
    int colWidth = getContentSize().width / RES;
    int rowHeight = getContentSize().height / RES;
    for (int c = 0; c < RES; c++) {
        Vec2 o = Vec2(c * colWidth, 0);
        Vec2 d = Vec2(c * colWidth, getContentSize().height);
        drawNode->drawLine(o, d, Color4F(1, 1, 1, 0.14f));
    }
    for (int r = 0; r < RES; r++) {
        Vec2 o(0, r * rowHeight);
        Vec2 d(getContentSize().width, r * rowHeight);
        drawNode->drawLine(o, d, Color4F(1, 1, 1, 0.14f));
    }
}

void World::createInput() {
    // Touch
    auto touch = EventListenerTouchOneByOne::create();
    touch->onTouchBegan = [this](Touch* touch, Event* event) {
        Util::touch_down = true;
        Util::touch_location = touch->getLocation();
        return true;
    };
    touch->onTouchMoved = [this](Touch* touch, Event* event) {
        Util::touch_location = touch->getLocation();
    };
    touch->onTouchCancelled = [](Touch* touch, Event* event) {
        Util::touch_down = false;
    };
    touch->onTouchEnded = [=](Touch* touch, Event* event) {
        Util::touch_down = false;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
}

void World::updateShips() {

}

void World::handleSpawns(float delta) {
    // Spawn Ships
    while (ships.size() < Player::ship_count) {
        addShip();
    }
    while (ships.size() > Player::ship_count) {
        removeShip();
    }

    // Spawn Bits
    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        auto& info = Player::bit_info[type];
        if (info.level < 1 /*|| info.spawned == info.capacity*/) continue;

        info.timer += delta;
        if (info.timer >= info.spawnTime) {
            if (info.spawned < info.capacity) {
                info.spawned++;
            }
            else {
                // Keep?
                Player::addBits(Player::calculateValue(type) / 2.0f);
            }
            info.timer = 0;
        }

        while (bits[type].size() < Player::bit_info[type].spawned) {
            addBit(type);
        }
    }
}

void World::handleCollisions() {
    for (auto s = ships.begin(); s != ships.end(); s++) {
        for (int i = 0; i < BitType::All; i++) {
            auto type = BitType(i);
            auto& bitVector = bits[type];
            for (auto b = bitVector.begin(); b != bitVector.end(); b++) {
                Vec2 diff = (*s)->getPosition() - (*b)->getPosition();
                if (diff.getLengthSq() < 441) {
                    auto& info = Player::bit_info[type];
                    if (info.level == 0) {
                        Player::bits++;
                    }
                    else {
                        Player::addBits(Player::calculateValue(type));
                        info.spawned--;
                    }
                    removeChild(*b);
                    b = bitVector.erase(b);
                    if (b == bitVector.end()) break;
                }
            }
        }
    }
}