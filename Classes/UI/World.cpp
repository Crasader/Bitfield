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
    for (int i = 0; i < Player::squadrons[0].ints["count"]; i++) {
        addShip();
    }
    followShip(false);
}

void World::onExit() {
    Layer::onExit();
    unscheduleUpdate();
}

void World::update(float delta) {
    Layer::update(delta);

    // Touch and Hold event
    if (Util::touch_down) {
        Util::touch_location = Util::capVector(Util::touch_location_original - getPosition(), 0, 0, WORLD_WIDTH, WORLD_HEIGHT);
    }
    handleSpawns(delta);
    handleCollisions();

    auto drawNode = getChildByName<DrawNode*>("drawNode");
    auto ship = ships.at(0);
    if (drawNode && ship) {
        drawNode->clear();
        drawNode->setPosition(ship->getPosition());
        drawNode->drawCircle(Vec2(0, 0), ship->vision_radius, 0, 64, false, Color4F::WHITE);
        drawNode->drawCircle(Vec2(0, 0), ship->separation_radius, 0, 64, false, Color4F::WHITE);

        // Project future position
        Vec2 projection = ship->velocity;
        projection.normalize();
        projection.scale(ship->wander_length);

        // Point to a random spot on the circle
        Vec2 toRadius = Vec2(ship->wander_radius, 0);
        toRadius = toRadius.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(ship->wander_theta));

        drawNode->drawLine(Vec2(0, 0), projection, Color4F::RED);
        drawNode->drawCircle(projection, ship->wander_radius, 0, 64, false, Color4F::RED);
        drawNode->drawLine(Vec2(0, 0), projection + toRadius, Color4F::BLUE);
    
        if (ship->getClosestBit()) {
            drawNode->drawLine(Vec2(0, 0), ship->getClosestBit()->getPosition() - ship->getPosition(), Color4F::GREEN);
        }

        drawNode->drawLine(Vec2(0, 0), ship->acceleration * 1000, Color4F::YELLOW);
    }
}

cocos2d::Vector<Ship*>& World::getShips() {
    return ships;
}

void World::addShip() {
    auto ship = Wanderer::create(Player::squadrons[0]);
    ship->setNeighbours(&ships);
    ship->setBits(&bits);
    ship->setBoundary(Rect(0, 0, WORLD_WIDTH, WORLD_HEIGHT));

    if (ships.empty()) {
        addChild(DrawNode::create(), 0, "drawNode");
        ship->setPosition(getContentSize() / 2.0f);
    }
    else {
        ship->setPosition(ships.at(0)->getPosition() + Vec2(64, 64) * rand_minus1_1());
        ship->w_seek = 0;
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
    drawNode->drawSolidRect(Vec2(-WORLD_OFFSET, -730 - WORLD_OFFSET),
        Size(WORLD_WIDTH + WORLD_OFFSET, WORLD_HEIGHT + 730 + WORLD_OFFSET), Color4F(WORLD_COLOR));
    addChild(drawNode, 0);

    // Create grid
    drawNode->setLineWidth(1);
    const int RES = GRID_RESOLUTION;
    int colWidth = WORLD_WIDTH / RES;
    int rowHeight = WORLD_HEIGHT / RES;
    for (int c = 0; c <= RES; c++) {
        Vec2 o = Vec2(c * colWidth, 0);
        Vec2 d = Vec2(c * colWidth, getContentSize().height);
        drawNode->drawLine(o, d, Color4F(1, 1, 1, 0.14f));
    }
    for (int r = 0; r <= RES; r++) {
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
        Util::touch_location = Util::capVector(touch->getLocation(), 0, 0, WORLD_WIDTH, WORLD_HEIGHT);
        Util::touch_location_original = touch->getLocation();
        return true;
    };
    touch->onTouchMoved = [this](Touch* touch, Event* event) {
        Util::touch_location = Util::capVector(touch->getLocation(), 0, 0, WORLD_WIDTH, WORLD_HEIGHT);
        Util::touch_location_original = touch->getLocation();
    };
    touch->onTouchCancelled = [](Touch* touch, Event* event) {
        Util::touch_down = false;
    };
    touch->onTouchEnded = [=](Touch* touch, Event* event) {
        Util::touch_down = false;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
}

void World::handleSpawns(float delta) {
    // Ships
    if (ships.size() < Player::squadrons[0].ints["count"]) {
        addShip();
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

void World::followShip(bool centered) {
    if (ships.empty()) return;
    auto ship = ships.at(0);
    int offset = 0;
    stopAllActions();
    if (!centered) {
        offset = 350;
    }

    auto follow = Follow::createWithOffset(ship, 0, offset,
        Rect(-WORLD_OFFSET, -(730 + WORLD_OFFSET),
            WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + 730 + WORLD_OFFSET * 2));
    runAction(follow);
}