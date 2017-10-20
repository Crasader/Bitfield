#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "cocos2d.h"
#include "Player.h"
#include "UI/UIWidget.h"

class Upgrade : public cocos2d::ui::Widget
{
public:
    Upgrade(int id);
    static Upgrade* create(int id);
    virtual bool init() override;
    virtual void update(float delta) override;

    int getID();

private:
    int id;

    void addBackground();
    void addIcon();
    void addName();
    void addDescription();
    void addBuyButton();
};

#endif // __UPGRADE_H__
