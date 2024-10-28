#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include "Entity.h"
#include "Camera.h"
#include <iostream>

class Enemy : public Entity 
{
public:
	Enemy(glm::ivec2 pos, 
		glm::ivec2 quad_size,
		std::shared_ptr<TileMap> tilemap,
	    std::shared_ptr<ShaderProgram> shader_program,
		std::string&& texture_path,
		glm::vec2 size_in_texture,
		std::shared_ptr<Camera> camera,
		std::shared_ptr<Player> player);

	virtual void update(int delta_time) override;

	virtual void collideWithEntity(Collision collision) override;

	// Defines behaviour on death
	virtual void onDeath() = 0;

	virtual EntityType getType() const override { return EntityType::Enemy; }

protected:
	// Called when the enemy is enabled (visible, etc)
	virtual void enable();

	// Called when the enemy is disabled (non-visible, defeated)
	virtual void disable();

	// The camera
	std::shared_ptr<Camera> m_camera;

	// The player
	std::shared_ptr<Player> m_player;

	// True iff the enemy was visible last frame
	bool m_was_visible = false;

	// True iff the enemy is visible this frame
	bool m_is_visible = false;
};

// A horse that has a spring in its base and jumps
// When it is on screen, goes at the player indefinitely
// Set up a periodic timed event that checks if the horse is dead and respawns it if conditions are met
class SpringHorse : public Enemy 
{
public:
	SpringHorse(glm::ivec2 pos,
		std::shared_ptr<TileMap> tilemap,
		std::shared_ptr<ShaderProgram> shader_program,
		std::string&& texture_path,
		glm::vec2 size_in_texture,
		std::shared_ptr<Camera> camera,
		std::shared_ptr<Player> player);

	virtual void update(int delta_time) override;

protected:
	virtual void enable() override;

	virtual void disable() override;

	virtual void onDeath() override;

private:
	enum Animation 
	{
		DEATH = 0,
		PREPARE_JUMP,
		JUMPING,
		FALLING
	};

	bool m_jumping = false;
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
