#ifndef __SHIP_H__
#define __SHIP_H__

#include "cocos2d.h"
#include "Types.h"
#include "UI/World.h"

class Bit;
typedef std::vector< std::vector < cocos2d::Vector< Bit* > > > Grid;

class Ship : public cocos2d::Sprite
{
public:
    Ship(SquadronInfo info, int squadronID, int shipID);
    static Ship* create(SquadronInfo info, int squadronID, int shipID);

    virtual void update(float delta) override;
    virtual void calculateForces(float delta);
    virtual void handleCollisions();
    virtual void onBitPickup();
    void applyForce(cocos2d::Vec2 force, float scale = 1);

    cocos2d::Vec2 seek(cocos2d::Vec2 target, bool slowdown = false);
    cocos2d::Vec2 align();
    virtual cocos2d::Vec2 cohesion();
    virtual cocos2d::Vec2 separate();
    virtual cocos2d::Vec2 wander();
    virtual cocos2d::Vec2 seekBits();
    cocos2d::Vec2 avoidWalls();
    cocos2d::Vec2 followLeader();

    void setNeighbours(cocos2d::Vector<Ship*>* neighbours);
    void setBits(Grid* bits);
    void setBoundary(cocos2d::Rect boundary);

    bool canSee(cocos2d::Node* target);
    bool inRange(cocos2d::Node* target);
    const cocos2d::Vec2& getVelocity();
    const cocos2d::Vec2& getAcceleration();
    Bit* getTargetBit();
    const std::string& getType();
    cocos2d::Vec2 getCenterOfSquadron();

    // Public for now
    cocos2d::Vec2 velocity;
    cocos2d::Vec2 acceleration;

    int count;
    int vision_radius;
    int separation_radius;
    int wall_separation_distance;
    int wander_length;
    int wander_radius;
    int wander_theta;
    int wander_delta;

    double scale;
    double max_speed;
    double max_force;
    double w_alignment;
    double w_cohesion;
    double w_separation;
    double w_wander;
    double w_seek;
    double w_seek_bits;
    double w_avoid_wall;
    double w_stay_grouped;

    int squadronID;
    int shipID;
    bool point_to_velocity;

protected:
    cocos2d::Vector<Ship*>* neighbours;
    Grid* bits;
    cocos2d::Rect boundary;

    std::string type;
    std::string sprite;

private:
    // TODO: send event to WORLD
    void addValuePopup(Bit* bit);
};

#endif // __SHIP_H__
