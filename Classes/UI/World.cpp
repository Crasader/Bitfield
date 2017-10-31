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
    }
    else {
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
    createCamera();
    initBits();
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

    updateBackground();
    updateGrid();
    updateFleet(delta);
    updateBits(delta);
    updateCamera();
}

void World::updateBackground()
{
    auto parallax = getChildByName("parallax");

    //std::function<void(Node*, Vec2, int, Color4F, float)> spawnPoly;
    auto spawnPoly = [&](Node* layer, Vec2 pos, int size, Color4F color, float a) {
        auto poly = DrawNode::create();
        poly->drawSolidPoly(POLYGON_VERTS, 4, color);
        poly->setContentSize(Size(2, 2));
        poly->setAnchorPoint(VEC_CENTER);
        poly->setPositionNormalized(pos);
        poly->setOpacity(0);
        poly->setScale(0);

        poly->runAction(Sequence::create(
            Spawn::create(
                EaseExponentialInOut::create(ScaleTo::create(2.f, size)),
                EaseExponentialInOut::create(FadeTo::create(2.f, 255 * a)),
                nullptr
            ),
            DelayTime::create(0.75f),
            Spawn::create(
                EaseExponentialInOut::create(ScaleTo::create(1.f, 0)),
                EaseExponentialInOut::create(FadeOut::create(1.f)),
                nullptr
            ),
            RemoveSelf::create(),
            nullptr
        ));
        layer->addChild(poly);
    };

    Ship* ship = nullptr;
    if (!fleet.empty() && !fleet.at(0).first.empty())
        ship = fleet.at(0).first.at(0);
    if (!ship) return;

    auto addPolygons = [=](const std::string& name, int limit,
        float chance, int sizeMin, int sizeDelta, float alpha)
    {
        auto layer = parallax->getChildByName(name);
        if (layer->getChildrenCount() < limit && rand_0_1() > chance) {
            auto pos = Vec2(ship->getPosition().x / WORLD_WIDTH - 0.075f + rand_0_1() * 0.15f,
                ship->getPosition().y / WORLD_HEIGHT - 0.075f + rand_0_1() * 0.15f);
            auto color = Color4F(Player::bit_info[BitType(cocos2d::random() % 7)].color);
            auto size = sizeMin + rand_0_1() * sizeDelta;
            spawnPoly(layer, pos, size, color, alpha);
        }
    };

    addPolygons("layer1", 4, 0.9f, 150, 50, 0.35f);
    addPolygons("layer2", 6, 0.88f, 75, 25, 0.3f);
    addPolygons("layer3", 8, 0.86f, 25, 25, 0.25f);
    addPolygons("layer4", 10, 0.84f, 10, 15, 0.2f);
}

void World::updateCamera()
{
    if (fleet[0].first.empty()) return;
    auto ship = fleet[0].first.at(0);
    auto camera = getChildByName("camera");
    camera->setPosition(ship->getPosition());
}

void World::updateGrid()
{
    // Remove dead bits or insert into grid
    for (auto it = bits.begin(); it != bits.end(); ++it) {
        auto bit = *it;
        if (bit->isRemoved()) {
            bits_spawned[bit->getType()]--;
            bit->removeFromParent();

            auto pos = bit->getPosition();
            int row = pos.x / GRID_SIZE;
            int col = pos.y / GRID_SIZE;
            grid.at(row).at(col).eraseObject(bit);

            it = bits.erase(it);
            if (it == bits.end()) break;
        }
        else {
            // Hide off-camera bits
            if (cameraContains(bit->getPosition())) {
                bit->setVisible(true);
            }
            else {
                bit->setVisible(false);
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
                streak->setFastMode(true);
                streaks.pushBack(streak);
                addChild(streak);
            }
        };

        // Ships don't exist in the world, so spawn them
        if (ships.empty()) {
            addShips(ships, streaks, count);
            if (squadronID == 0) offsetCameraForPanelIsVisible(true);
        }
        else if (ships.size() < count) {
            // Need more ships, add to world
            addShips(ships, streaks, count);
        }

        // Update streaks
        for (int shipID = 0; shipID < streaks.size(); shipID++) {
            auto ship = ships.at(shipID);
            auto streak = streaks.at(shipID);
            streak->setPosition(ship->getPosition());
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

        if (bits_spawned[type] < Player::bit_info[type].spawned) {
            addBit(type);
        }
    }
}

void World::addBit(BitType type) {
    auto bit = Bit::create(type);
    bit->setPosition(getContentSize().width * (0.005f + rand_0_1() * 0.99f),
        getContentSize().height * (0.005f + rand_0_1() * 0.99f));
    addChild(bit, 88);
    bits.pushBack(bit);
    bits_spawned[type]++;

    // Add to grid
    auto pos = bit->getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;
    grid.at(row).at(col).pushBack(bit);
}

void World::offsetCameraForPanelIsVisible(bool visible) {
    if (fleet[0].first.empty()) return;
    auto ship = fleet[0].first.at(0);
    auto camera = getChildByName("camera");

    stopAllActions();
    if (visible) {
        cameraOffset = 350;
    }
    else {
        cameraOffset = 0;
    }

    auto follow = Follow::createWithOffset(camera, 0, cameraOffset,
        Rect(-WORLD_OFFSET, -WORLD_OFFSET - 730,
            WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + (730 + WORLD_OFFSET) * 2));
    runAction(follow);
}

bool World::cameraContains(cocos2d::Vec2 point)
{
    auto rect = Rect(-getPosition(), Size(GAME_WIDTH, GAME_HEIGHT));
    return (rect.containsPoint(point));
}

void World::createBackground() {
    auto parallax = ParallaxNode::create();
    parallax->setPositionNormalized(VEC_CENTER);
    parallax->setAnchorPoint(Vec2(0.5f, 0.5f));
    parallax->setIgnoreAnchorPointForPosition(false);
    addChild(parallax, 0, "parallax");

    // Create Background and Grid
    {
        auto drawNode = DrawNode::create(1);
        drawNode->drawSolidRect(Vec2(-WORLD_OFFSET, -WORLD_OFFSET - 730), Vec2(WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + (WORLD_OFFSET + 730) * 2), Color4F(WORLD_COLOR));

        drawNode->setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
        drawNode->setAnchorPoint(Vec2(0.5f, 0.5f));
        Color4F lineColor(1, 1, 1, 0.2f);
        for (int x = 0; x <= WORLD_WIDTH; x += WORLD_WIDTH / VISUAL_GRID_RESOLUTION) {
            Vec2 o = Vec2(x, 0);
            Vec2 d = Vec2(x, getContentSize().height);
            drawNode->drawLine(o, d, lineColor);

            for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / VISUAL_GRID_RESOLUTION) {
                auto nodeSize = 4;
                drawNode->drawSolidRect(Vec2(x - nodeSize, y - nodeSize),
                    Vec2(x + nodeSize, y + nodeSize), lineColor);//drawSolidCircle(o + Vec2(0, GRID_SIZE * i), 2, 0, 4, Color4F(1, 1, 1, 0.14f));
            }
        }
        for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / VISUAL_GRID_RESOLUTION) {
            Vec2 o(0, y);
            Vec2 d(getContentSize().width, y);
            drawNode->drawLine(o, d, lineColor);
        }
        drawNode->setName("layer0");
        parallax->addChild(drawNode, -99, Vec2(1, 1), Vec2(0, 0));
    }

    // Add parallax layers
    auto addParallaxLayer = [=](const std::string& name, int z, float scale) {
        auto layer = Layer::create();
        layer->setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
        layer->setAnchorPoint(Vec2(0.5f, 0.5f));
        layer->setIgnoreAnchorPointForPosition(false);
        layer->setName(name);
        parallax->addChild(layer, z, Vec2(scale, scale),
            Vec2(GAME_WIDTH * 0.5f * (1 - scale), GAME_HEIGHT * 0.5f * (1 - scale)));
    };
    addParallaxLayer("layer1", -1, 0.96f);
    addParallaxLayer("layer2", -2, 0.9f);
    addParallaxLayer("layer3", -3, 0.84f);
    addParallaxLayer("layer4", -4, 0.78f);
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

void World::createCamera()
{
    auto camera = Node::create();
    addChild(camera, 0, "camera");
    cameraOffset = 350;
}

void World::initBits()
{
    for (int i = 0; i < 7; i++) {
        bits_spawned[i] = 0;
    }

    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        auto& info = Player::bit_info[type];
        if (info.level == 0) continue;
        while (bits_spawned[type] < Player::bit_info[type].spawned) {
            addBit(type);
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