#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	explicit Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos, 
		            std::shared_ptr<ShaderProgram> shader_program);
	
	// Updates the player
	void update(int delta_time);
	
	// Renders the player
	void render();

	// Sets the player's position
	void setPosition(glm::vec2 const& pos);
	
private:
	// True iff the player is jumping
	bool m_is_jumping = false;
	
	glm::ivec2 m_tilemap_displ; 

	// The player's position
	glm::ivec2 m_pos;

	int m_jump_angle; 

	int m_start_y;

	// The spritesheet
	std::shared_ptr<Texture> m_spritesheet;
	
	// A pointer to the sprite
	std::shared_ptr<Sprite> m_sprite;

	// A pointer to the tilemap
	std::shared_ptr<TileMap> m_tilemap;

};


#endif // _PLAYER_INCLUDE


