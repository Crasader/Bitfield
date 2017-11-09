#pragma once

#include <cocos2d.h>
#include "ui/UIButton.h"
#include "Types.h"

class SquadronCard: public cocos2d::ui::Button
{
public:
    static SquadronCard* create(SquadronInfo* info);
    bool init(SquadronInfo* info);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) override;

private:
    SquadronInfo* info;

    void createIconBackground();
    void createIcon();
    void createLabel();
    void createInfo();
    void createEventListener();
};