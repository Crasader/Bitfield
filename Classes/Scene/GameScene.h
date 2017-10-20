#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class World;
class HUD;

class GameScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(GameScene);
    virtual ~GameScene();

    virtual bool init() override;
    virtual void update(float delta) override;

private:
    // Initialization
    void createWorld();
    void createHUD();
};

#endif // __GAME_SCENE_H__
