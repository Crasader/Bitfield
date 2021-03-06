#pragma once

#include <cocos2d.h>

class FleetPanel : public cocos2d::Node
{
public:
    CREATE_FUNC(FleetPanel);
    virtual bool init() override;
    virtual void update(float delta) override;

private:
    // Initialize
    void createBackground();
    void createSquadronSlots();
    
    void createCenterPanel();
    cocos2d::Node* createSquadronRank(int rank);

    void createButtons();
    void createEventListeners();

    void updateButtons();
};

