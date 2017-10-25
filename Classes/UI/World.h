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

    void followShip(bool centered);

private:
    cocos2d::Vector<Ship*> ships;
    std::map< BitType, cocos2d::Vector< Bit* > > bits;

    void createInput();
    void createGrid();

    void handleSpawns(float delta);
    void debugShip();
};

#endif // __WORLD_H__
