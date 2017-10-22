#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include "UI/UIWidget.h"

class Generator : public cocos2d::ui::Widget
{
public:
    Generator(BitType id);
    static Generator* create(BitType id);
    virtual bool init() override;
    virtual void update(float delta) override;

private:
    BitType id;

    // Initialize
    void addBackground();
    void addLevelBar();
    void addIcon();
    void addName();
    void addSpawnBar();
    void addSpawnCapacity();
    void addBuyButton();
    void addLevelUp();

    // Update
    void updateLevelBar();
    void updateSpawnBar();
    void updateInfo();
    void updateBuyButton();
};

#endif // __GENERATOR_H__
