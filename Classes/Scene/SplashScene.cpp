#include "SplashScene.h"
#include "GameScene.h"
#include "Constants.h"
#include "PlayerData/Player.h"

#include <sstream>
#include <iomanip>

USING_NS_CC;

static bool transitioning = false;

bool SplashScene::init() {
    if (!Scene::init()) {
        return false;
    }
    scheduleUpdate();

    createBackground();
    createLabels();
    return true;
}

void SplashScene::update(float delta) {
    const float LOAD_TIME = 1.2f;

    if (transitioning) return;

    totalTime += delta;
    if (totalTime >= LOAD_TIME) {
        totalTime = LOAD_TIME;

        // Load
        Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
        Player::load();
        auto nextScene = GameScene::create();

        // Fade out each letter
        auto title = getChildByName<Label*>("title");
        for (int i = 0; i < title->getStringLength(); i++) {
            auto letter = title->getLetter(i);
            auto actions = Sequence::createWithTwoActions(DelayTime::create(0.05f * i), FadeOut::create(0.15f));
            letter->runAction(actions);
        }

        Director::getInstance()->replaceScene(TransitionFade::create(1.f, nextScene));
        transitioning = true;
    }
}

void SplashScene::createBackground() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create solid rect as background, fade in
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(origin, Vec2(visibleSize.width, visibleSize.height), Color4F(WORLD_COLOR));
    drawNode->setOpacity(0);
    drawNode->runAction(EaseSineInOut::create(FadeIn::create(0.25f)));
    addChild(drawNode, 0, "drawNode");
}

void SplashScene::createLabels() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Title
    auto title = Label::createWithTTF("Bitfield", FONT_DEFAULT, 128);
    title->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height  * 0.52f));
    title->setCascadeOpacityEnabled(true);
    addChild(title, 0, "title");

    // Fade in each letter
    for (int i = 0; i < title->getStringLength(); i++) {
        auto letter = title->getLetter(i);
        letter->setOpacity(0);
        auto actions = Sequence::createWithTwoActions(DelayTime::create(0.25f + 0.05f * i), FadeIn::create(0.15f));
        letter->runAction(actions);
    }
}