#include "World.h"

#include "Util.h"
#include "Input.h"
#include "Constants.h"
#include "Scene\GameScene.h"
#include "GameObject\SquadronFactory.h"
#include "GameObject\Bit.h"
#include "GameObject\Ship.h"
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
    setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
    setCascadeOpacityEnabled(true);

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
    updateBackground();
    updateGrid();
    updateFleet(delta);
    updateBits(delta);
    updateCamera();
}

void World::updateBackground()
{
    auto parallax = getChildByName("parallax");
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
            DelayTime::create(1.f),
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

    addPolygons("layer1", 6, 0.9f, 150, 40, 0.55f);
    addPolygons("layer2", 8, 0.88f, 75, 45, 0.5f);
    addPolygons("layer3", 10, 0.86f, 35, 15, 0.45f);
    addPolygons("layer4", 14, 0.86f, 10, 10, 0.4f);
}

void World::updateCamera()
{
    if (fleet[0].first.empty()) return;
    auto ship = fleet[0].first.at(0);
    auto camera = getChildByName("camera");
    auto cameraOffset = getChildByName("cameraOffset");
    camera->setPosition(ship->getPosition() - cameraOffset->getPosition());
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
                auto streak = MotionStreak::create(2.0f, 0, 8, Color3B(Player::bit_info[BitType(colorIndex)].color), info.strings["streak"]);
                streak->setFastMode(true);
                streaks.pushBack(streak);
                addChild(streak);
            }
        };

        // Ships don't exist in the world, so spawn them
        if (ships.empty()) {
            addShips(ships, streaks, count);
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
        if (info.level < 1) continue;

        info.timer += delta;
        if (info.timer >= info.spawnTime) {
            if (info.spawned < info.capacity) {
                info.spawned++;
            }
            else {
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

static int num_squares = 0;
void World::addGridSquare(cocos2d::Vec2 pos)
{
    if (!worldContains(pos) || num_squares > 10) return;

    auto square = DrawNode::create();
    square->drawSolidRect(Vec2(0, 0), Vec2(GRID_SIZE, GRID_SIZE), Color4F::WHITE);
    square->setPosition(getPositionInGrid(pos));
    square->setOpacity(255 * 0.2f);
    square->runAction(Sequence::create(
        EaseSineInOut::create(FadeOut::create(0.5f)),
        RemoveSelf::create(),
        CallFunc::create([&]() {
            num_squares--;
        }),
        nullptr
        ));
    addChild(square);
    num_squares++;
}

void World::offsetCameraForPanelIsVisible(bool visible) {
    auto cameraOffset = getChildByName("cameraOffset");
    if (visible) {
        cameraOffset->runAction(EaseBackOut::create(MoveTo::create(0.3f, Vec2(0, 350))));
    }
    else {
        cameraOffset->runAction(EaseSineOut::create(MoveTo::create(0.3f, VEC_ZERO)));
    }
}

bool World::cameraContains(cocos2d::Vec2 point)
{
    auto rect = Rect(-getPosition(), Size(GAME_WIDTH, GAME_HEIGHT));
    return (rect.containsPoint(point));
}

bool World::worldContains(cocos2d::Vec2 point)
{
    return point.x > 0
        && point.x < WORLD_WIDTH
        && point.y > 0
        && point.y < WORLD_HEIGHT;
}

cocos2d::Vec2 World::getCellInGrid(cocos2d::Vec2 pos)
{
    int gridX = pos.x / GRID_SIZE;
    int gridY = pos.y / GRID_SIZE;
    return Vec2(gridX, gridY);
}

cocos2d::Vec2 World::getPositionInGrid(cocos2d::Vec2 pos)
{
    auto gridPos = getCellInGrid(pos);
    gridPos.scale(GRID_SIZE);
    return gridPos;
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
        drawNode->drawSolidRect(Vec2(-WORLD_OFFSET, -WORLD_OFFSET - 730),
            Vec2(WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + (WORLD_OFFSET + 730) * 2),
            Color4F(WORLD_COLOR));

        drawNode->setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
        drawNode->setAnchorPoint(Vec2(0.5f, 0.5f));
        Color4F lineColor(1, 1, 1, 0.2f);
        for (int x = 0; x <= WORLD_WIDTH; x += WORLD_WIDTH / GRID_RESOLUTION) {
            Vec2 o = Vec2(x, 0);
            Vec2 d = Vec2(x, getContentSize().height);
            drawNode->drawLine(o, d, lineColor);

            for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / GRID_RESOLUTION) {
                auto nodeSize = 4;
                drawNode->drawSolidRect(Vec2(x - nodeSize, y - nodeSize),
                    Vec2(x + nodeSize, y + nodeSize), lineColor);
            }
        }
        for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / GRID_RESOLUTION) {
            Vec2 o(0, y);
            Vec2 d(getContentSize().width, y);
            drawNode->drawLine(o, d, lineColor);
        }
        drawNode->setName("layer0");
        parallax->addChild(drawNode, -5, Vec2(1, 1), Vec2(0, 0));
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
    touch->onTouchBegan = [=](Touch* touch, Event* event) {
        return true;
    };
    touch->onTouchMoved = [=](Touch* touch, Event* event) {
        return true;
    };
    touch->onTouchCancelled = [=](Touch* touch, Event* event) {
        return true;
    };
    touch->onTouchEnded = [=](Touch* touch, Event* event) {
        if (Input::touch_time < Input::TAP_TIME) {
            addGridSquare(touch->getLocation() - getPosition());
        }
        return true;
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
    // Always follow the camera
    auto camera = Node::create();
    addChild(camera, 0, "camera");

    auto follow = Follow::createWithOffset(camera, 0, 0,
        Rect(-WORLD_OFFSET, -WORLD_OFFSET - 730,
            WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + (730 + WORLD_OFFSET) * 2));
    runAction(follow);

    // Offset node (THE HACKS ARE REAL)
    auto cameraOffset = Node::create();
    addChild(cameraOffset, 0, "cameraOffset");
}

void World::initBits()
{
    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        auto& info = Player::bit_info[type];
        bits_spawned[i] = 0;
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