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

bool World::init()
{
    if (!Layer::init()) return false;
    scheduleUpdate();
    setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
    setCascadeOpacityEnabled(true);

    createBackground();
    createInput();
    createGrid();
    createCamera();
    initBits();

    return true;
}

void World::update(float delta) {
    Layer::update(delta);
    updateBackground();
    updateGrid();
    updateFleet(delta);
    updateBits(delta);
    updateCamera();
}

void World::createPolygon(const std::string& layerName, Vec2 pos, int limit, int sizeMin,
    int sizeDelta, float alpha, float spread) {
    auto parallax = getChildByName("parallax");
    auto layer = parallax->getChildByName(layerName);
    if (layer->getChildrenCount() >= limit || rand_0_1() < 0.95f) return;

    auto randPos = Vec2(pos.x / WORLD_WIDTH - spread / 2 + rand_0_1() * spread,
        pos.y / WORLD_HEIGHT - spread / 2 + rand_0_1() * spread);
    auto color = Color4F(Player::generators[BitType(cocos2d::random() % 7)].color);
    auto size = sizeMin + rand_0_1() * sizeDelta;

    // Create a polygon
    auto poly = DrawNode::create();
    poly->drawSolidPoly(POLYGON_VERTS, 4, color);
    poly->setContentSize(Size(2, 2));
    poly->setAnchorPoint(VEC_CENTER);
    poly->setPositionNormalized(randPos);
    poly->setOpacity(0);
    poly->setScale(0);

    poly->runAction(Sequence::create(
        Spawn::create(
            EaseExponentialInOut::create(ScaleTo::create(2.f, size)),
            EaseExponentialInOut::create(FadeTo::create(2.f, 255 * alpha)),
            nullptr
        ),
        DelayTime::create(0.8f + rand_0_1() * 0.4f),
        Spawn::create(
            EaseExponentialInOut::create(ScaleTo::create(1.f, 0)),
            EaseExponentialInOut::create(FadeOut::create(1.f)),
            nullptr
        ),
        RemoveSelf::create(),
        nullptr
    ));
    layer->addChild(poly);
}

void World::updateBackground()
{
    Ship* ship = nullptr;
    if (!fleet.empty() && !fleet.at(0).first.empty())
        ship = fleet.at(0).first.at(0);
    if (!ship) return;

    //if (rand_0_1() < 0.6f) return;
    createPolygon("layer1", ship->getPosition(), 6, 180, 240, 0.4f, 0.175f);
    createPolygon("layer2", ship->getPosition(), 6, 95, 45, 0.35f, 0.15f);
    createPolygon("layer3", ship->getPosition(), 6, 35, 25, 0.25f, 0.125f);
    createPolygon("layer4", ship->getPosition(), 6, 10, 10, 0.2f, 0.05f);
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
    // For each slot,
    for (int squadronID = 0; squadronID < 7; squadronID++) {
        if (!Player::isSlotUnlocked(squadronID)) continue;
        const auto& type = Player::squadrons_equipped[squadronID];
        auto& info = Player::squadrons[type];

        auto& ships = fleet[squadronID].first;
        auto& streaks = fleet[squadronID].second;
        
        // Clear out of date squadrons
        if (!ships.empty()) {
            auto ship = ships.at(0);
            if (type != ship->getType()) {
                ships.clear();
            }
        }

        // Add ships
        auto& count = info.ints["count"];
        if (ships.size() < count) {
            cocos2d::log("%d %s", squadronID, type.c_str());
            for (int shipID = ships.size(); shipID < count; shipID++) {
                auto ship = SquadronFactory::createShipWithInfo(info, squadronID, shipID);
                ship->setNeighbours(&ships);
                ship->setBits(&grid);
                ship->setBoundary(Rect(Vec2(0, 0), getContentSize()));

                if (squadronID == 0 && shipID == 0) {
                    ship->setPosition(Vec2(WORLD_WIDTH * 0.5f, WORLD_HEIGHT * 0.5f));
                }
                else if (ships.empty()) {
                    ship->setPosition(Vec2(rand_0_1() * WORLD_WIDTH, rand_0_1() * WORLD_HEIGHT));
                }
                else {
                    ship->setPosition(ships.at(0)->getPosition());
                }

                ships.pushBack(ship);
                addChild(ship, 99);

                // Also add streak
                auto colorIndex = shipID % 7;
                auto color = Color3B(Player::generators[BitType(colorIndex)].color);
                auto streak = MotionStreak::create(2.0f, 0, 8, color, info.strings["streak"]);
                streak->setFastMode(true);
                streaks.pushBack(streak);
                addChild(streak);
            }
        }

        // Update streaks
        for (int shipID = 0; shipID < streaks.size(); shipID++) {
            auto ship = ships.at(shipID);
            auto streak = streaks.at(shipID);
            streak->setPosition(ship->getPosition());
        }
    }
    if (DEBUG_SHIP) debugShip();
}

void World::updateBits(float delta) {
    // Spawn Bits
    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        auto& info = Player::generators[type];
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

        if (bits_spawned[type] < Player::generators[type].spawned) {
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
    cameraOffset->stopAllActions();
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

    // Add parallax layers
    auto addLayer = [=](const std::string& name, int z, float scale) {
        auto layer = Layer::create();
        layer->setContentSize(Size(WORLD_WIDTH, WORLD_HEIGHT));
        layer->setAnchorPoint(Vec2(0.5f, 0.5f));
        layer->setIgnoreAnchorPointForPosition(false);
        layer->setName(name);
        parallax->addChild(layer, z, Vec2(scale, scale),
            Vec2(GAME_WIDTH * 0.5f * (1 - scale), GAME_HEIGHT * 0.5f * (1 - scale)));
    };

    addLayer("layer0", -99, 1);
    addLayer("layer1", -1, 0.96f);
    addLayer("layer2", -2, 0.9f);
    addLayer("layer3", -3, 0.84f);
    addLayer("layer4", -4, 0.78f);

    createBackgroundGrid();
}

void World::createBackgroundGrid()
{
    auto parallax = getChildByName("parallax");
    auto layer = parallax->getChildByName("layer0");

    // Background
    auto backgroundColor = DrawNode::create();
    backgroundColor->drawSolidRect(Vec2(-WORLD_OFFSET, -WORLD_OFFSET - 730),
        Vec2(WORLD_WIDTH + WORLD_OFFSET * 2, WORLD_HEIGHT + (WORLD_OFFSET + 730) * 2),
        Color4F(WORLD_COLOR));
    layer->addChild(backgroundColor);

    // Vertical Lines
    auto verticalLines = DrawNode::create(1);
    for (int x = 0; x <= WORLD_WIDTH; x += WORLD_WIDTH / GRID_RESOLUTION) {
        Vec2 o = Vec2(x, 0);
        Vec2 d = Vec2(x, -getContentSize().height);
        verticalLines->drawLine(o, d, GRID_COLOR);
    }
    verticalLines->setPositionY(getContentSize().height);
    verticalLines->setScaleY(0.35f);
    verticalLines->runAction(EaseSineInOut::create(ScaleTo::create(5.5f, 1)));
    layer->addChild(verticalLines);

    // Horizontal Lines
    auto horizontalLines = DrawNode::create(1);
    for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / GRID_RESOLUTION) {
        Vec2 o(0, y);
        Vec2 d(getContentSize().width, y);
        horizontalLines->drawLine(o, d, GRID_COLOR);
    }
    horizontalLines->setScaleX(0.4f);
    horizontalLines->runAction(EaseSineInOut::create(ScaleTo::create(5.5f, 1)));
    layer->addChild(horizontalLines);

    // Grid Dots
    auto dots = DrawNode::create(1);
    for (int x = 0; x <= WORLD_WIDTH; x += WORLD_WIDTH / GRID_RESOLUTION) {
        Vec2 o = Vec2(x, 0);
        Vec2 d = Vec2(x, getContentSize().height);

        // Dots along the lines
        for (int y = 0; y <= WORLD_HEIGHT; y += WORLD_HEIGHT / GRID_RESOLUTION) {
            auto nodeSize = 4;
            dots->drawSolidRect(Vec2(x - nodeSize, y - nodeSize),
                Vec2(x + nodeSize, y + nodeSize), GRID_COLOR);
        }
    }
    dots->setOpacity(0);
    dots->runAction(Sequence::create(
        DelayTime::create(1.5f),
        EaseSineInOut::create(FadeIn::create(0.5f)),
        nullptr
    ));
    layer->addChild(dots);
}

void World::createInput() {
    // Touch
    auto touch = Input::createTouchListener();
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
    for (int i = 0; i < GRID_SIZE; i++) {
        grid.push_back(std::vector<cocos2d::Vector< Bit* > >());
        for (int j = 0; j < GRID_SIZE; j++) {
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
        auto& info = Player::generators[type];
        bits_spawned[i] = 0;
        if (info.level == 0) continue;
        while (bits_spawned[type] < Player::generators[type].spawned) {
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