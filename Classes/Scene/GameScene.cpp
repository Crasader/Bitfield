#include "GameScene.h"

#include "Input.h"
#include "Util.h"
#include "SplashScene.h"
#include "UI\World.h"
#include "UI\HUD.h"

USING_NS_CC;

bool GameScene::init() {
    if (!Scene::init()) return false;
    scheduleUpdate();

    createInput();
    createWorld();
    createHUD();
    return true;
}

GameScene::~GameScene() {
    Player::save();
}

void GameScene::update(float delta) {
    Scene::update(delta);
    updateInput(delta);
}

void GameScene::updateInput(float delta)
{
    if (Input::touch_down) {
        Input::touch_time += delta;
    }
}

void GameScene::createInput()
{
    // Touch
    auto touch = EventListenerTouchOneByOne::create();
    touch->onTouchBegan = [this](Touch* touch, Event* event) {
        //cocos2d::log("Touch Position: %.2f, %.2f", touch->getLocation().x, touch->getLocation().y);
        Input::touch_down = true;
        Input::touch_pos = touch->getLocation();
        Input::touch_start = touch->getLocation();
        Input::touch_time = 0;
        return true;
    };
    touch->onTouchMoved = [this](Touch* touch, Event* event) {
        Input::touch_pos = touch->getLocation();
        return true;
    };
    touch->onTouchCancelled = [](Touch* touch, Event* event) {
        Input::touch_down = false;
        return true;
    };
    touch->onTouchEnded = [=](Touch* touch, Event* event) {
        Input::touch_down = false;
        Input::touch_end = touch->getLocation();
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

    // Handle Exit
    auto keyboard = EventListenerKeyboard::create();
    keyboard->onKeyPressed = [this](EventKeyboard::KeyCode code, Event* event) {
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
            Director::getInstance()->end();
        }
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
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
