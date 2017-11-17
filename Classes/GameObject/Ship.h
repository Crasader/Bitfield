#pragma once

#include "cocos2d.h"
#include "Types.h"

class World;
class Bit;
class ShipStreak;

class Ship : public cocos2d::Sprite
{
public:
    static Ship* create(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual bool init(World* world, SquadronInfo& info, int squadronID, int shipID);

    virtual void update(float delta) override;
    void applyForce(cocos2d::Vec2 force, float scale = 1);
    bool inRange(cocos2d::Node* target);
    virtual bool canSee(cocos2d::Node* target);
    virtual Bit* getTargetBit();

    // Behaviours
    virtual cocos2d::Vec2 seek(cocos2d::Vec2 target, bool slowdown = false);
    virtual cocos2d::Vec2 align();
    virtual cocos2d::Vec2 cohesion();
    virtual cocos2d::Vec2 separate();
    virtual cocos2d::Vec2 wander();
    virtual cocos2d::Vec2 seekBits();
    virtual cocos2d::Vec2 avoidWalls();
    virtual cocos2d::Vec2 leash();

    // Getters
    cocos2d::Vec2 getCenterOfSquadron();
    const cocos2d::Vec2& getVelocity();
    const cocos2d::Vec2& getAcceleration();
    const std::string& getType();
    const std::string& getSprite();
    ShipStreak* getStreak();
    virtual cocos2d::Color3B getStreakColor();

protected:
    virtual void loadInfo(World* world, SquadronInfo& info, int squadronID, int shipID);
    virtual void loadStreak(SquadronInfo& info);
    
    virtual void calculateForces(float delta);
    virtual void handleCollisions();
    virtual void onBitPickup();

    World* world;
    ShipStreak* streak;

    int squadronID;
    int shipID;
    cocos2d::Vec2 velocity;
    cocos2d::Vec2 acceleration;
    bool point_to_velocity;

    std::string type;
    std::string sprite;

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
    double w_leash;
};