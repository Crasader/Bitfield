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
    static cocos2d::Node* createIconLabel(int iconType, double amount, double size);

    static std::string jsonToString(rapidjson::Document &jsonObject);
    static rapidjson::Document loadDocument(const std::string& path);

    static std::string getFormattedDouble(double bits);
    static std::string getSuffix(int exponent);

    static cocos2d::Vec2 capVector(cocos2d::Vec2 v, double xMin, double yMin, double xMax, double yMax);

    
    static bool touch_down;
    static cocos2d::Vec2 touch_location;
    static cocos2d::Vec2 touch_location_original;
};

