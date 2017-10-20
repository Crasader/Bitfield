#pragma once

#include <cocos2d.h>

class PurchaseButton: public cocos2d::Node
{
public:
    enum CostType {
        Bits, Diamonds
    };

    static PurchaseButton* create(cocos2d::Size size);
    bool init(cocos2d::Size size);

    void setHeaderColor(const cocos2d::Color4B& color);
    void setHeader(const std::string& header);
    void setCost(double amount);
    void setCostType(CostType type);

private:
    void addButton();
    void addHeaderBackground();
    void addHeader();
    void addCost();

    std::string header;
    std::string cost;
    CostType type;
};

