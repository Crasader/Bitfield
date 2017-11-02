#include "Input.h"

USING_NS_CC;

const float Input::TAP_TIME = 0.2f;

bool Input::touch_down = false;
cocos2d::Vec2 Input::touch_pos = Vec2();
cocos2d::Vec2 Input::touch_start = Vec2();
cocos2d::Vec2 Input::touch_end = Vec2();
float Input::touch_time = 0;