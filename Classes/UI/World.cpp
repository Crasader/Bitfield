#include "World.h"

#include "Util.h"
#include "Input.h"
#include "Constants.h"
#include "Scene\GameScene.h"
#include "GameObject\SquadronFactory.h"
#include "GameObject\Bit.h"
#include "GameObject\Ship.h"
#include "GameObject\ShipStreak.h"
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
    createEventListeners();
    initBits();
    fleet = std::vector<Squadron>(7);

    return true;
}

static float totalDelta = 0; // TODO
void World::update(float delta) {
    Layer::update(delta);
    updateBackground();
    updateGrid();
    totalDelta += delta;
    if (totalDelta > 0.65f) {
        updateFleet(delta);
    }
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
    if (!fleet.empty() && !fleet[Player::slot_selected].empty())
        ship = fleet[Player::slot_selected].at(0);
    if (!ship) return;

    //if (rand_0_1() < 0.6f) return;
    createPolygon("layer1", ship->getPosition(), 6, 180, 240, 0.4f, 0.175f);
    createPolygon("layer2", ship->getPosition(), 6, 95, 45, 0.35f, 0.15f);
    createPolygon("layer3", ship->getPosition(), 6, 35, 25, 0.25f, 0.125f);
    createPolygon("layer4", ship->getPosition(), 6, 10, 10, 0.2f, 0.05f);
}

void World::updateCamera()
{
    if (fleet[Player::slot_selected].empty()) return;
    auto ship = fleet[Player::slot_selected].at(0);
    auto camera = getChildByName("camera");
    auto cameraOffset = getChildByName("cameraOffset");
    camera->setPosition(ship->getPosition() - cameraOffset->getPosition());
}

void World::updateGrid()
{
    // Remove dead bits
    for (auto bit : bits) {
        if (!bit->isActive()) continue;
        if (bit->isCollected()) {
            // Remove from grid
            auto pos = bit->getPosition();
            int row = pos.x / GRID_SIZE;
            int col = pos.y / GRID_SIZE;
            grid.at(row).at(col).eraseObject(bit);

            removeBit(bit);
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
    for (int squadronID = 0; squadronID < 7; squadronID++) {
        if (!Player::isSlotUnlocked(squadronID)) continue;
        const auto& type = Player::squadrons_equipped[squadronID];
        auto& ships = fleet[squadronID];

        // Clear out of date squadrons
        if (!ships.empty()) {
            auto ship = ships.at(0);
            if (type != ship->getType()) {
                for (auto ship : ships) {
                    auto streak = ship->getStreak();
                    if (streak) streak->removeFromParent();
                    ship->removeFromParent();
                }
                ships.clear();
            }
        }
        if (type == "Empty") continue;

        // Add ships
        auto& info = Player::squadrons[type];
        auto& count = info.ints["count"];
        if (ships.size() < count) {
            for (int shipID = ships.size(); shipID < count; shipID++) {
                auto ship = SquadronFactory::createShipWithInfo(this, info, squadronID, shipID);

                if (ships.empty()) {
                    ship->setPosition(Vec2(rand_0_1() * WORLD_WIDTH, rand_0_1() * WORLD_HEIGHT));
                }
                else {
                    ship->setPosition(ships.at(0)->getPosition());
                }

                ships.pushBack(ship);
                addChild(ship, 99);
                auto streak = ship->getStreak();
                if (streak) addChild(ship->getStreak(), 98);
            }
        }

        // Hide off-camera ships
        for (int shipID = 0; shipID < ships.size(); shipID++) {
            auto ship = ships.at(shipID);
            
            if (!cameraContains(ship->getBoundingBox())) {
                ship->setVisible(false);
            }
            else if (!ship->isVisible()){
                ship->setVisible(true);
            }
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
    Bit* bit = nullptr;

    // End of free list, add to pool
    if (free_list[type] == nullptr) {
        bit = createBit(type);
    }
    else {
        // Get a bit from the free list, update pointer
        bit = free_list[type];
        free_list[type] = bit->next;
    }

    // Activate the bit randomly in the world
    bit->setPosition(getContentSize().width * (0.005f + rand_0_1() * 0.99f),
        getContentSize().height * (0.005f + rand_0_1() * 0.99f));
    bit->setActive(true);
    bit->setCollected(false);
    bits_spawned[type]++;

    // Add the bit to grid data structure
    auto pos = bit->getPosition();
    int row = pos.x / GRID_SIZE;
    int col = pos.y / GRID_SIZE;
    grid.at(row).at(col).pushBack(bit);
}

void World::removeBit(Bit* bit)
{
    auto type = bit->getType();
    Player::generators[type].spawned--;
    bits_spawned[type]--;
    bit->setActive(false);
    bit->setCollected(false);
    
    // Update free list
    bit->next = free_list[type];
    free_list[type] = bit;
}

void World::addTileGlow(int x, int y, Color3B color, float a)
{
    auto pos = getCellPosition(x, y, true);
    if (!cameraContains(pos)) return;;

    auto square = DrawNode::create();
    square->drawSolidRect(Vec2(-GRID_SIZE /2 , -GRID_SIZE /2 ), Vec2(GRID_SIZE/2, GRID_SIZE/2), Color4F(color));
    square->setAnchorPoint(VEC_CENTER);
    square->setPosition(pos);
    square->setOpacity(255 * a);
    square->runAction(Sequence::create(
        EaseSineInOut::create(
            Spawn::createWithTwoActions(
                FadeOut::create(0.4f),
                ScaleTo::create(0.4f, 0.75f)
            )
        ),
        RemoveSelf::create(),
        nullptr
    ));
    addChild(square);
}

void World::consumeTile(int x, int y)
{
    if (!gridContains(x, y)) return;
    for (auto bit : grid.at(x).at(y)) {
        bit->setCollected(true);
        Player::dispatchEvent(EVENT_BIT_PICKUP, (void*)bit);
    }
}

void World::offsetCamera(bool visible) {
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
    auto worldRect = Rect(-getPosition(), Size(GAME_WIDTH, GAME_HEIGHT));
    return (worldRect.containsPoint(point));
}

bool World::cameraContains(cocos2d::Rect rect)
{
    auto worldRect = Rect(-getPosition(), Size(GAME_WIDTH, GAME_HEIGHT));
    return worldRect.intersectsRect(rect);
}

bool World::worldContains(cocos2d::Vec2 point)
{
    return point.x > getPositionX()
        && point.x < getPositionX() + WORLD_WIDTH
        && point.y > getPositionY()
        && point.y < getPositionY() + WORLD_HEIGHT;
}

bool World::gridContains(int x, int y)
{
    return x >= 0 && x < GRID_RESOLUTION && y >= 0 && y < GRID_RESOLUTION;
}

const Squadron& World::getSquadron(int id)
{
    return fleet[id];
}

const Bits& World::getBits(int x, int y)
{
    return grid.at(x).at(y);
}

cocos2d::Vec2 World::getCellIndex(cocos2d::Vec2 pos)
{
    int gridX = pos.x / GRID_SIZE;
    int gridY = pos.y / GRID_SIZE;
    return Vec2(gridX, gridY);
}

cocos2d::Vec2 World::getCellPosition(int x, int y, bool center)
{
    auto gridPos = Vec2(x, y);
    gridPos.scale(GRID_SIZE);
    if (center) {
        gridPos += Vec2(GRID_SIZE / 2.f, GRID_SIZE / 2.f);
    }
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
    // Touch events - highlight tapped tiles
    auto touch = Input::createTouchListener();
    touch->onTouchEnded = [=](Touch* touch, Event* event) {
        if (Input::touch_time < Input::TAP_TIME) {
            // Find what grid square we touched
            auto cell = getCellIndex(touch->getLocation() - getPosition());
            int x = cell.x;
            int y = cell.y;
            if (gridContains(x, y))
                addTileGlow(x, y, Color3B::WHITE, 0.2f);
            //consumeTile(x, y); // IF (TOUCH_RELIC_UNLOCKED) ...
        }
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
}

void World::createGrid()
{
    for (int i = 0; i < GRID_RESOLUTION; i++) {
        grid.push_back(std::vector< Bits >(600));
        for (int j = 0; j < GRID_RESOLUTION; j++) {
            grid.at(i).push_back(Bits());
        }
    }
}

void World::createCamera()
{
    // Always follow the camera
    auto camera = Node::create();
    camera->setPositionNormalized(VEC_CENTER);
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
    // Create the current maximum number of bits (add more later if needed)
    int count = 0;
    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        auto& info = Player::generators[type];

        int start = count;
        for (int j = 0; j < info.capacity; j++) {
            createBit(type);
            count++;
        }

        // Set up the free list for this bit type
        free_list[i] = bits.at(start);
        for (int j = 0; j < count - start - 1; j++) {
            bits.at(j)->next = bits.at(j + 1);
        }
        bits.at(count - start - 1)->next = nullptr;

        // Set up live bits
        bits_spawned[i] = 0;
        if (info.level == 0) continue;
        while (bits_spawned[i] < Player::generators[type].spawned) {
            addBit(type);
        }
    }
}

Bit * World::createBit(BitType type)
{
    auto bit = Bit::create(type);
    bit->setActive(false);
    bits.pushBack(bit);
    addChild(bit, 88);
    return bit;
}

void World::createEventListeners()
{
    // Popup on bit pickup
    auto l_bit_pickup = EventListenerCustom::create(EVENT_BIT_PICKUP, [=](EventCustom* event) {
        auto bit = static_cast<Bit*>(event->getUserData());
        if (!cameraContains(bit->getPosition())) return;

        auto info = Player::generators[bit->getType()];
        auto popup = Label::createWithTTF(info.valueString, FONT_DEFAULT, 52);
        auto c = Color3B(info.color);
        popup->setColor(Color3B(c.r * 1.25f, c.g * 1.25f, c.b * 1.25f));
        popup->setPosition(bit->getPosition());
        popup->setOpacity(0);
        popup->runAction(Sequence::create(
            EaseSineInOut::create(
                Spawn::createWithTwoActions(
                    FadeIn::create(0.15f),
                    ScaleTo::create(0.15f, 1.3f)
                )
            ),
            DelayTime::create(0.5f),
            EaseSineInOut::create(
                Spawn::createWithTwoActions(
                    FadeOut::create(0.15f),
                    ScaleTo::create(0.15f, 0.5f)
                )
            ),
            DelayTime::create(0.3f),
            RemoveSelf::create(),
            nullptr)
        );
        addChild(popup, 150 + bit->getType());
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_bit_pickup, this);
}

void World::debugShip()
{
    //auto drawNode = getChildByName<DrawNode*>("drawNode");
    //auto ship = fleet[0].first.at(0);
    //if (!drawNode) {
    //    addChild(DrawNode::create(), 0, "drawNode");
    //}
    //if (drawNode && ship) {
    //    drawNode->clear();
    //    drawNode->setPosition(ship->getPosition());
    //    drawNode->drawCircle(Vec2(0, 0), ship->vision_radius, 0, 64, false, Color4F::WHITE);
    //    drawNode->drawCircle(Vec2(0, 0), ship->separation_radius, 0, 64, false, Color4F::WHITE);

    //    // Project future position
    //    Vec2 projection = ship->velocity;
    //    projection.normalize();
    //    projection.scale(ship->wander_length);

    //    // Point to a random spot on the circle
    //    Vec2 toRadius = Vec2(ship->wander_radius, 0);
    //    toRadius = toRadius.rotateByAngle(Vec2(0, 0), CC_DEGREES_TO_RADIANS(ship->wander_theta));

    //    drawNode->drawLine(Vec2(0, 0), projection, Color4F::RED);
    //    drawNode->drawCircle(projection, ship->wander_radius, 0, 64, false, Color4F::RED);
    //    drawNode->drawLine(Vec2(0, 0), projection + toRadius, Color4F::BLUE);

    //    drawNode->drawLine(Vec2(0, 0), ship->acceleration * 1000, Color4F::YELLOW);

    //    auto sep = ship->separate();
    //    drawNode->drawLine(Vec2(0, 0), sep * 1000, Color4F::GREEN);
    //}
}