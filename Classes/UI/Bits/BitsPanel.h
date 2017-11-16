#pragma once

#include <cocos2d.h>
#include "Types.h"

class BitsPanel : public cocos2d::Node
{
public:
    enum View {
        Generators, Upgrades
    };

    CREATE_FUNC(BitsPanel);
    virtual bool init() override;
    virtual void update(float delta) override;

    void setView(View view);

private:
    // Initialize
    void createBackground();
    void addTabs();
    
    void addGeneratorLayer();
    void addGenerators();
    void addGenerator(BitType type);
    void addBuyAmountButton();

    void addUpgradeLayer();
    void addUpgrades();

    void createEventListeners();

    // Update
    void updateIndicator();

    View currentView;
};

