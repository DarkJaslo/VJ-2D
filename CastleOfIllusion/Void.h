#ifndef _VOID_INCLUDE
#define _VOID_INCLUDE

#include "Entity.h"

// Represents a fall outside the level. Acts like a trigger collider for the player and other entities to know that they
// have gone out of bounds
class Void : public Entity 
{
public:
	// Constructor: give size and position
	Void(glm::ivec2 upleft_corner_pos, glm::ivec2 collision_size, std::shared_ptr<ShaderProgram> shader_program);

	// Update function that does nothing (we have nothing to update)
	virtual void update(int delta_time) final override {}

	// Render function that does nothing (we have nothing to render)
	virtual void render() final override {}

	// Collide with entity function that does nothing
	virtual void collideWithEntity(Collision collision) final override {}

	virtual EntityType getType() const final override { return EntityType::Void; }
};

#endif // _VOID_INCLUDE
