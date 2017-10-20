#ifndef __HUD_H__
#define __HUD_H__

#include "cocos2d.h"

class World;

class HUD : public cocos2d::Layer
{
public:
    enum PanelID {
        Bits, Upgrades, Squadron, Artifacts, Shop
    };

    static HUD* create();
    virtual bool init();
    void setWorld(World* world);

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float delta);

    void setPanel(PanelID id);

private:
    World* world;
    PanelID currentPanel;

    void addBitCounter();
    void addPanels();
    void addTabs();
};

#endif // __HUD_H__
