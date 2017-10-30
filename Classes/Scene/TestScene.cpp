#include "TestScene.h"

#include "Constants.h"

USING_NS_CC;

static bool transitioning = false;

bool TestScene::init() {
    if (!Scene::init())  return false;
    scheduleUpdate();

    createBackground();
    createSprites();
    return true;
}

void TestScene::update(float delta) {
    Scene::update(delta);

}

void TestScene::createBackground() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create solid rect as background, fade in
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(origin, Vec2(visibleSize.width, visibleSize.height), Color4F(WORLD_COLOR));
    addChild(drawNode, 0, "drawNode");
}

void TestScene::createSprites()
{
    for (int i = 0; i < 20000; i++) {
        auto sprite = Sprite::create(SPRITE_BIT);
        sprite->setPosition(Vec2(-GAME_WIDTH, -GAME_HEIGHT));
        addChild(sprite, 1);
    }
}
