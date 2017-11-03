#pragma once

#include <cocos2d.h>

class Input
{
public:
    static const float TAP_TIME;

    static bool touch_down;
    static cocos2d::Vec2 touch_pos;
    static cocos2d::Vec2 touch_start;
    static cocos2d::Vec2 touch_end;
    static float touch_time;

    static cocos2d::EventListenerTouchOneByOne* createTouchListener();
    static cocos2d::EventListenerTouchOneByOne* createTouchUpdater();
};

