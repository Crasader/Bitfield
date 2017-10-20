#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "cocos2d.h"

class SplashScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(SplashScene);
    virtual bool init();
    virtual void update(float delta);

private:
    float totalTime;
    void createBackground();
    void createLabels();
};

#endif // __SPLASH_SCENE_H__
