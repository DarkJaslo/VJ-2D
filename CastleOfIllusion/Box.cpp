#include "Box.h"

Box::Box(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program) 
	:
		ThrowableTile(pos,
			tilemap,
			{ 0, 0 },
			shader_program,
			"images/Blocks2.png",
			{ 0.125f, 0.5f }, /* size_in_texture */
			{ 0.25f, 0.5f }, /* pos_in_texture */
			true /* destroyed_on_impact */
		)
{
	m_start_pos = pos;

	m_thrown = false;
	m_affected_by_x_drag = false;
	m_bounces = false;

	// Temporal
	m_affected_by_gravity = true;
	m_enabled = true;
}

void Box::collideWithEntity(Collision collision) 
{
	ThrowableTile::collideWithEntity(collision);
}

void Box::setEnabled(bool enabled) 
{
	ThrowableTile::setEnabled(enabled);

	m_affected_by_gravity = enabled;
	if (enabled)
	{
		m_pos = m_start_pos;
		m_can_collide = true;
		m_can_collide_with_tiles = true;
		m_thrown = false;
	}
}