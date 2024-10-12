#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"

// Represents a player.

class Player : public Entity
{

public:
	explicit Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos, 
		            std::shared_ptr<ShaderProgram> shader_program);
	
	// Updates the player
	virtual void update(int delta_time) final override;
	void updateMovement(int delta_time);
	
private:
	// True iff the player feet are on the floor
	bool m_is_grounded = false;
	
	glm::ivec2 m_tilemap_displ;

	int m_jump_angle; 

	int m_start_y;

	// Player's movement acceleration
	glm::vec2 m_acc;

	// Calculates the velocity needed for the player to jump to height
	float calculateJumpVelocity(float height, float gravity);
};

#endif // _PLAYER_INCLUDE
