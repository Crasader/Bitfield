#pragma once

#include <cocos2d.h>
#include "UI/UIImageView.h"
#include "UI/UIButton.h"

class Util
{
public:
    static cocos2d::ui::ImageView* createRoundedRect(const std::string& path,
        cocos2d::Size size, cocos2d::Color4B color);

    static cocos2d::ui::Button* createRoundedButton(const std::string& path,
        cocos2d::Size size, cocos2d::Color4B color);

private:
    Util();
};

