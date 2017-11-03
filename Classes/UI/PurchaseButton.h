#pragma once

#include <cocos2d.h>
#include "ui/UIButton.h"

class PurchaseButton: public cocos2d::ui::Button
{
public:
    enum IconType {
        Bits, Diamonds
    };

    static PurchaseButton* create(const std::string& path,
        cocos2d::Size size, IconType iconType);
    bool init(const std::string& path, cocos2d::Size size, IconType iconType);

    void setButtonColor(const cocos2d::Color4B& color);
    void setHeaderColor(const cocos2d::Color4B& color);
    void setHeader(const std::string& header);
    void setCost(const std::string& cost);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) override;

    std::function<void()> onPurchase;

private:
    void addHeaderBackground();
    void addHeader();
    void addCost(IconType iconType);
};

