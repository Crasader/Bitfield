#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include <map>
#include <tuple>
#include <vector>

class Ship;
class Bit;

typedef cocos2d::Vector< Bit* > Bits;
typedef cocos2d::Vector< Ship* > Squadron;
typedef std::vector< Squadron > Fleet;
typedef std::vector< std::vector < cocos2d::Vector< Bit* > > > Grid;

class World : public cocos2d::Layer
{
public:
    static World* create();
    virtual bool init() override;
    virtual void update(float delta);

    void addBit(BitType type);
    void removeBit(Bit* bit);
    
    void offsetCamera(bool offset);
    bool cameraContains(cocos2d::Vec2 point);
    bool cameraContains(cocos2d::Rect rect);
    bool worldContains(cocos2d::Vec2 point);
    bool gridContains(int x, int y);

    const Squadron& getSquadron(int id);
    const Bits& getBits(int x, int y);

    // Get the cell of a position (Vec(300, 10) --> Vec(1, 0))
    cocos2d::Vec2 getCellIndex(cocos2d::Vec2 pos);
    cocos2d::Vec2 getCellPosition(int x, int y, bool center = false);
    void addTileGlow(int x, int y, cocos2d::Color3B color, float a, float scale = 1);
    void consumeTile(int x, int y);

private:
    Fleet fleet;
    Bits bits;
    Grid grid;
    int bits_spawned[7];
    Bit* free_list[7];

    float cameraOffset;
    int cameraTarget;

    void createBackground();
    void createBackgroundGrid();
    void createInput();
    void createGrid();
    void createCamera();
    void createPolygon(const std::string& layerName, cocos2d::Vec2 pos,
        int limit, int sizeMin, int sizeDelta, float alpha, float spread);
    void initBits();
    Bit* createBit(BitType type);
    void createEventListeners();

    void updateBackground();
    void updateCamera();
    void updateGrid();
    void updateFleet(float delta);
    void updateBits(float delta);
    void debugShip();
};

#endif // __WORLD_H__
