#pragma once

#include <cocos2d.h>
#include "UI/UIImageView.h"
#include "UI/UIButton.h"

#include "rapidjson/document.h"

class Util
{
public:
    static cocos2d::ui::ImageView* createRoundedRect(const std::string& path,
        cocos2d::Size size, cocos2d::Color4B color);
    static cocos2d::ui::Button* createRoundedButton(const std::string& path,
        cocos2d::Size size, cocos2d::Color4B color);

    static std::string Util::jsonToString(rapidjson::Document &jsonObject);
    static rapidjson::Document Util::loadDocument(const std::string& path);

    static std::string getFormattedDouble(double bits);
    static std::string getSuffix(int exponent);

    static bool touch_down;
    static cocos2d::Vec2 touch_location;
};

