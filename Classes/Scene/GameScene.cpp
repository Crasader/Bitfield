#include "GameScene.h"
#include "SplashScene.h"

#include "ui\UILayout.h"

#include "..\Constants.h"
#include "..\UI\World.h"
#include "..\UI\HUD.h"

USING_NS_CC;

bool GameScene::init() {
    if (!Scene::init()) {
        return false;
    }
    scheduleUpdate();

    // Handle Exitting
    auto keyboard = EventListenerKeyboard::create();
    keyboard->onKeyPressed = [this](EventKeyboard::KeyCode code, Event* event) {
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
            Director::getInstance()->end();
        }
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    createWorld();
    createHUD();
    return true;
}

GameScene::~GameScene() {
    Player::save();
}

void GameScene::update(float delta) {
    Scene::update(delta);
}

void GameScene::createWorld() {
    auto world = World::create();
    addChild(world, 0, "world");
}

void GameScene::createHUD() {
    auto hud = HUD::create();
    hud->setWorld(getChildByName<World*>("world"));
    addChild(hud, 1, "hud");
}