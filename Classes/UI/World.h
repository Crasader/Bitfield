#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include <map>
#include <tuple>

class Ship;
class Bit;

typedef std::map<int,
    std::pair< cocos2d::Vector<Ship*>,
               cocos2d::Vector< cocos2d::MotionStreak* > > > Fleet;
typedef std::map< BitType, cocos2d::Vector< Bit* > > Bits;
typedef std::vector< std::vector< cocos2d::Vector< Bit* > > > Grid;

class World : public cocos2d::Layer
{
public:
    static World* create();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float delta);

    void addBit(BitType type);
    void offsetCamera(bool offset);

private:
    Fleet fleet;
    Bits bits;
    Grid grid;

    void createBackground();
    void createInput();
    void createGrid();

    void updateGrid();
    void updateFleet(float delta);
    void updateBits(float delta);

    void debugShip();
};

#endif // __WORLD_H__
