#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include "Entity.h"

class Enemy : public Entity 
{
public:
	Enemy() = default;

	virtual void collideWithEntity(Collision collision) override;

	virtual EntityType getType() const override { return EntityType::Enemy; }

private:
	// (re)spawn position
};

// A horse that has a spring in its base and jumps
// When it is on screen, goes at the player indefinitely
// Set up a periodic timed event that checks if the horse is dead and respawns it if conditions are met
class SpringHorse : public Enemy 
{

};

// A projectile produced by the monkey's gong
// Has a limited lifetime, manage with TimedEvent
class GongProjectile : public Entity 
{
public:

private:
};

// A monkey with a gong
class GongMonkey : public Enemy 
{

};

#endif // _ENEMY_INCLUDE
