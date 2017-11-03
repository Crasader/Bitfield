#pragma once

#include <cocos2d.h>

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
    void addBuyAmountButton();

    void addUpgradeLayer();
    void addUpgrades();

    // Update
    void updateIndicator();

    View currentView;
};

