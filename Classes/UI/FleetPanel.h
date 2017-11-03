#pragma once

#include <cocos2d.h>

class FleetPanel : public cocos2d::Node
{
public:
    CREATE_FUNC(FleetPanel);
    virtual bool init() override;

private:
    // Initialize
    void createBackground();
    void createSquadronSlots();
    void createCenterPanel();
    void createButtons();
};

