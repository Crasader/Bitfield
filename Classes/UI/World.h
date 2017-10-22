#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include <map>

class Ship;
class Bit;

class World : public cocos2d::Layer
{
public:
    static World* create();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float delta);

    void addShip();
    void removeShip();

    void addBit(BitType type);
    void removeBit(BitType type);

    cocos2d::Vector<Ship*>& getShips();

private:
    cocos2d::Camera* camera;
    cocos2d::Vector<Ship*> ships;
    std::map< BitType, cocos2d::Vector< Bit* > > bits;

    void createInput();
    void createGrid();
    void createLights();

    void touchHoldCallback();

    void updateShips();
    void handleSpawns(float delta);
    void handleCollisions();
};

#endif // __WORLD_H__
