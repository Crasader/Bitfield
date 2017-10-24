#pragma once

#include <cocos2d.h>

class SquadronPanel : public cocos2d::Node {
public:
    static SquadronPanel* create();
    virtual bool init() override;
    virtual void update(float delta) override;

private:
    // Initialize
    void addBackground();
    void addPurchaseButton();
    void addSilhouettes();

    void updatePurchaseButton();
};