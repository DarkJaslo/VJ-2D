#ifndef _CAMERA_POINT_INCLUDE
#define _CAMERA_POINT_INCLUDE

#include "Entity.h"
#include "Camera.h"
#include "Player.h"
#include "Boss.h"

// Represents a fall outside the level. Acts like a trigger collider for the player and other entities to know that they
// have gone out of bounds
class CameraPoint : public Entity
{
public:
	// Constructor: give size and position
	CameraPoint(glm::ivec2 upleft_corner_pos, 
		glm::ivec2 collision_size, 
		std::shared_ptr<Camera> camera, 
		std::shared_ptr<Player> player, 
		glm::ivec2 player_spawn_point, 
		int camera_offset, 
		std::shared_ptr<ShaderProgram> shader_program,
		int identifier,
		std::shared_ptr<Boss> boss);

	// Update function that does nothing (we have nothing to update)
	virtual void update(int delta_time) final override {}

	// Render function that does nothing (we have nothing to render)
	virtual void render() final override {}

	// Called when the entity collides with something
	virtual void collideWithEntity(Collision collision);

	virtual EntityType getType() const final override { return EntityType::CameraPoint; }

	virtual void setEnabled(bool enabled) override;

private:
	enum Identifier 
	{
		GreenToSky = 0,
		SkyToSand = 1,
		SandToBoss = 2,
		None
	};

	std::shared_ptr<Camera> m_camera;

	std::shared_ptr<Player> m_player;

	glm::ivec2 m_player_spawn_point;

	int m_camera_offset;

	Identifier m_id = None;

	std::shared_ptr<Boss> m_boss;
};

#endif // _CAMERA_POINT_INCLUDE
