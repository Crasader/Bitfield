#include "Input.h"

USING_NS_CC;

const float Input::TAP_TIME = 0.2f;

bool Input::touch_down = false;
Vec2 Input::touch_pos = Vec2();
Vec2 Input::touch_start = Vec2();
Vec2 Input::touch_end = Vec2();
float Input::touch_time = 0;

EventListenerTouchOneByOne* Input::createTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [=](Touch* touch, Event* event) {
        return true;
    };

    listener->onTouchMoved = [=](Touch* touch, Event* event) {
        return true;
    };

    listener->onTouchCancelled = [=](Touch* touch, Event* event) {
        return true;
    };

    listener->onTouchEnded = [=](Touch* touch, Event* event) {
        return true;
    };

    return listener;
}

EventListenerTouchOneByOne* Input::createTouchUpdater()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [](Touch* touch, Event* event) {
        //cocos2d::log("Touch Position: %.2f, %.2f", touch->getLocation().x, touch->getLocation().y);
        touch_down = true;
        touch_pos = touch->getLocation();
        touch_start = touch->getLocation();
        touch_time = 0;
        return true;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event) {
        touch_pos = touch->getLocation();
        return true;
    };

    listener->onTouchCancelled = [](Touch* touch, Event* event) {
        touch_down = false;
        return true;
    };

    listener->onTouchEnded = [=](Touch* touch, Event* event) {
        touch_down = false;
        touch_end = touch->getLocation();
        return true;
    };

    return listener;
}
