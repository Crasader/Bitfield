#ifndef __HUD_H__
#define __HUD_H__

#include "cocos2d.h"

class World;

class HUD : public cocos2d::Layer
{
public:
    enum PanelID {
        Bits, Fleet, Missions, Artifacts, Shop, Temp
    };

    static HUD* create();
    virtual bool init() override;
    virtual void update(float delta) override;

    void setWorld(World* world);
    
    void addPanel(cocos2d::Node* panel, PanelID id);
    void showPanel(PanelID id);
    void hidePanel(PanelID id);
    void togglePanel(PanelID id);

    void unlockFleet();

    cocos2d::Node* getTab(PanelID id);

private:
    World* world;

    void createCounter();
    void createPanels();
    void createTabs();
    void createEventListeners();
};

#endif // __HUD_H__
