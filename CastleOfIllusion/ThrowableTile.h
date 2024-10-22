#ifndef _THROWABLE_TILE_INCLUDE
#define _THROWABLE_TILE_INCLUDE

#include "Entity.h"

// Represents an object that acts like a tile, but can be picked up and thrown. Damages/destroys some entities.
// All throwable tiles have something in common:
//  1. When they are placed, players collide with them
//  2. They can be picked up
//  3. They can be thrown when picked up, which always damages enemies and can (or not) destroy this tile
class ThrowableTile : public Entity 
{
public:
	ThrowableTile(glm::ivec2 pos,
		std::shared_ptr<TileMap> tilemap,
		glm::ivec2 tilemap_pos,
		std::shared_ptr<ShaderProgram> shader_program,
		std::string&& texture_path,
		glm::vec2 size_in_texture,
		glm::vec2 position_in_texture,
		bool destroyed_on_impact);
	
	virtual void update(int delta_time) override;

	virtual void collideWithEntity(Collision collision) override;

	virtual EntityType getType() const override { return EntityType::ThrowableTile; }

	// Returns true iff it's on the ground without moving
	bool isStatic() const { return !m_picked_up && !m_thrown; }

	// Returns true iff it has been thrown (and hasn't touched ground)
	bool isBeingThrown() const { return m_thrown; }

	// Returns true iff this is destroyed on impact
	bool isDestroyedOnImpact() const { return m_destroyed_on_impact; }

	// To be called when being picked up by the player
	void onPickUp();

	// To be called when being thrown by the player
	void onThrow(bool looking_right, bool moving);

	// To be called when destroyed
	virtual void onDestroy();

protected:
	// True if this throwable tile is destroyed on impact with some other throwable tile or an attacking player
	bool m_destroyed_on_impact;

	// True while being picked up by the player
	bool m_picked_up = false;

	// True while being thrown or affected by physics in general
	bool m_thrown = false;
};

#endif // _THROWABLE_TILE_INCLUDE