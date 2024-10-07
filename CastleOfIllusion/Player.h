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
	
private:
	// True iff the player is jumping
	bool m_is_jumping = false;
	
	glm::ivec2 m_tilemap_displ;

	int m_jump_angle; 

	int m_start_y;
};

#endif // _PLAYER_INCLUDE
