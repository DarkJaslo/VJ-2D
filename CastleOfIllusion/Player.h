#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"

// Represents a player.

class Player : public Entity
{

public:
	Player(glm::vec2 const& pos, 
		   std::shared_ptr<TileMap> tilemap, 
		   glm::ivec2 const& tilemap_pos, 
		   std::shared_ptr<ShaderProgram> shader_program);
	
	// Updates the player
	virtual void update(int delta_time) final override;

	// Called when the player collides with something
	virtual void collideWithEntity(Collision collision) final override;
	
private:
	// Takes a hit from a damage source, losing 1 power and losing the "try" if no power is left
	void takeHit();

	// Gains power from eating cake
	void gainPower();

	// Gains points from coins or defeating enemies
	void gainPoints(unsigned int gain);


	// True iff the player is jumping
	bool m_is_jumping = false;
	
	glm::ivec2 m_tilemap_displ;

	int m_jump_angle; 

	int m_start_y;

	// The points the player currently has
	int m_points = 0;

	// The power (or health) the player currently has
	int m_power;

	// The maximum (and initial) amount of power. In the original game, can be increased up to 5
	int m_max_power = 3;
};

#endif // _PLAYER_INCLUDE
