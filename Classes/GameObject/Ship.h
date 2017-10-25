#ifndef __SHIP_H__
#define __SHIP_H__

#include "cocos2d.h"
#include "Types.h"

class Bit;

class Ship : public cocos2d::Sprite
{
public:
    Ship(SquadronInfo info);
    virtual void update(float delta) override;
    virtual void calculateForces();
    virtual void handleCollisions();

    void applyForce(cocos2d::Vec2 force, float scale = 1);
    cocos2d::Vec2 wander();
    cocos2d::Vec2 seek(cocos2d::Vec2 target, bool slowDown = false);
    cocos2d::Vec2 separate(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 cohesion(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 align(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 seekBits(std::map< BitType, cocos2d::Vector< Bit* > >& bits);
    cocos2d::Vec2 stayWithin(cocos2d::Rect boundary);
    cocos2d::Vec2 stayGrouped();

    bool isFront(const cocos2d::Vector<Ship*>& neighbours);
    bool canSee(cocos2d::Node* target);
    bool inRange(cocos2d::Node* target);
    const cocos2d::Vec2& getVelocity();
    const cocos2d::Vec2& getAcceleration();

    void setNeighbours(cocos2d::Vector<Ship*>* neighbours);
    void setBits(std::map< BitType, cocos2d::Vector< Bit* > >* bits);
    void setBoundary(cocos2d::Rect boundary);
    Bit* getTargetBit();
    cocos2d::Vec2 getCenterOfMass(const cocos2d::Vector<Ship*>& neighbours);

    cocos2d::Vec2 velocity;
    cocos2d::Vec2 acceleration;

    int vision_radius;
    int separation_radius;
    int wall_separation_distance;
    int wander_length;
    int wander_radius;
    int wander_theta;
    int wander_delta;

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

protected:
    cocos2d::Vector<Ship*>* neighbours;
    std::map< BitType, cocos2d::Vector< Bit* > >* bits;
    cocos2d::Rect boundary;

    std::string type;
    std::string sprite;

};

#endif // __SHIP_H__
