#pragma once

#include <cocos2d.h>
#include "ui/UIButton.h"

class SquadronSlot: public cocos2d::ui::Button
{
public:
    SquadronSlot(int slot);
    static SquadronSlot* create(int slot);
    bool init(const std::string& path);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) override;

    std::function<void()> onClick;

private:
    int slot;

    void createIconBackground();
    void createIcon();
    void createLabel();
    void createEventListener();
};

