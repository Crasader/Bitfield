#include "World.h"

#include "Util.h"
#include "..\Constants.h"
#include "..\Scene\GameScene.h"
#include "..\GameObject\SquadronFactory.h"
#include "..\GameObject\Bit.h"
#include "..\GameObject\Ship.h"
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
    setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));

    createBackground();
    createInput();
    createGrid();
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

    updateGrid();
    updateFleet(delta);
    updateBits(delta);
}

void World::updateGrid()
{
    // Remove dead bits or insert into grid
    for (auto& pair : bits) {
        auto& vec = pair.second;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            auto bit = *it;
            if (bit->isRemoved()) {
                bit->removeFromParent();

                auto pos = bit->getPosition();
                int row = pos.x / GRID_SIZE;
                int col = pos.y / GRID_SIZE;
                grid.at(row).at(col).eraseObject(bit);

                it = vec.erase(it);
                if (it == vec.end()) break;
            }
        }
    }
}

void World::updateFleet(float delta) {
    for (int squadronID = 0; squadronID < Player::squadron_slots; squadronID++) {
        auto info = Player::squadrons[squadronID];
        auto type = info.strings["type"];
        auto count = info.ints["count"];

        auto& ships = fleet[squadronID].first;
        auto& streaks = fleet[squadronID].second;
        auto addShips = [&](cocos2d::Vector<Ship*>& ships, Vector<MotionStreak*>& streaks, int count) {
            for (int shipID = ships.size(); shipID < count; shipID++) {
                // Add ship to vector
                auto ship = SquadronFactory::createShipWithInfo(info, squadronID, shipID);
                ship->setNeighbours(&ships);
                ship->setBits(&grid);
                ship->setBoundary(Rect(Vec2(0, 0), getContentSize()));

                if (ships.empty()) {
                    ship->setPosition(Vec2(WORLD_WIDTH / 2, WORLD_HEIGHT / 2));
                }
                else {
                    ship->setPosition(ships.at(0)->getPosition());
                }
                
                ships.pushBack(ship);
                addChild(ship, 99);

                // Also add streak
                auto colorIndex = shipID % 7;
                auto streak = MotionStreak::create(2.0f, 0, 8, Color3B(Player::bit_info[BitType(colorIndex)].color), SPRITE_STREAK);
                streak->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintBy::create(0.5f, 0, 30, 0), TintBy::create(0.5f, 0, -30, 0))));
                streaks.pushBack(streak);
                addChild(streak);
            }
        };

        // Ships don't exist in the world, so spawn them
        if (ships.empty()) {
            addShips(ships, streaks, count);
            if (squadronID == 0) offsetCamera(false);
        }
        else if (ships.size() < count) {
            // Need more ships, add to world
            addShips(ships, streaks, count);
        }

        // Update streaks
        for (int shipID = 0; shipID < streaks.size(); shipID++) {
            streaks.at(shipID)->setPosition(ships.at(shipID)->getPosition());
        }

        // TODO: Eventually will need to evict ships if type doesnt match here
    }

    if (DEBUG_SHIP) debugShip();
}

void World::updateBits(float delta) {
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

void World::addBit(BitType type) {
    auto bit = Bit::create(type);
    bit->setPosition(getContentSize().width * (0.02f + rand_0_1() * 0.96f),
        getContentSize().height * (0.02f + rand_0_1() * 0.96f));
    addChild(bit, 88);
    bits[type].pushBack(bit);

    // Add to grid
    auto pos = bit->getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;
    grid.at(row).at(col).pushBack(bit);
}

void World::offsetCamera(bool offset) {
    if (fleet.empty()) return;
    auto ship = fleet[0].first.at(0);
    int offsetAmount = 0;
    stopAllActions();
    if (!offset) {
        offsetAmount = 350;
    }

    auto follow = Follow::createWithOffset(ship, 0, offsetAmount,
        Rect(-WORLD_OFFSET, -(730 + WORLD_OFFSET),
            WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + 730 + WORLD_OFFSET * 2));
    runAction(follow);
}

void World::createBackground() {
    // Create Background
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2(-WORLD_OFFSET, -730 - WORLD_OFFSET),
        Size(WORLD_WIDTH + WORLD_OFFSET, WORLD_HEIGHT + 730 + WORLD_OFFSET), Color4F(WORLD_COLOR));
    addChild(drawNode, 0);

    // Create grid
    drawNode->setLineWidth(1);
    int numCols = WORLD_WIDTH / GRID_SIZE;
    int numRows = WORLD_HEIGHT / GRID_SIZE;
    for (int c = 0; c <= numCols; c++) {
        Vec2 o = Vec2(c * GRID_SIZE, 0);
        Vec2 d = Vec2(c * GRID_SIZE, getContentSize().height);
        drawNode->drawLine(o, d, Color4F(1, 1, 1, 0.14f));
        for (int i = 0; i <= GRID_SIZE; i++) {
            drawNode->drawSolidCircle(o + Vec2(0, GRID_SIZE * i), 2, 0, 6, Color4F(1, 1, 1, 0.14f));
        }
    }
    for (int r = 0; r <= numRows; r++) {
        Vec2 o(0, r * GRID_SIZE);
        Vec2 d(getContentSize().width, r * GRID_SIZE);
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

void World::createGrid()
{
    for (int i = 0; i < GRID_WIDTH; i++) {
        grid.push_back(std::vector<cocos2d::Vector< Bit* > >());
        for (int j = 0; j < GRID_HEIGHT; j++) {
            grid[i].push_back(cocos2d::Vector< Bit* >());
        }
    }
}

void World::debugShip()
{
    auto drawNode = getChildByName<DrawNode*>("drawNode");
    auto ship = fleet[0].first.at(0);
    if (!drawNode) {
        addChild(DrawNode::create(), 0, "drawNode");
    }
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

        drawNode->drawLine(Vec2(0, 0), ship->acceleration * 1000, Color4F::YELLOW);

        auto sep = ship->separate();
        drawNode->drawLine(Vec2(0, 0), sep * 1000, Color4F::GREEN);
    }
}