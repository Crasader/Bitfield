#pragma once

#include "cocos2d.h"

class TestScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(TestScene);
    virtual bool init();
    virtual void update(float delta);

private:
    void createBackground();
    void createSprites();
};
