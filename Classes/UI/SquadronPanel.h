#pragma once

#include <cocos2d.h>

class SquadronPanel : public cocos2d::Node {
public:
    CREATE_FUNC(SquadronPanel);
    virtual bool init() override;

private:
    // Initialize
    void addBackground();
    void addPurchaseButton();
    void addSilhouettes();
};