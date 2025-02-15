#include "Barrel.h"

#include <iostream>

Barrel::Barrel(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	glm::ivec2 tilemap_pos,
	std::shared_ptr<ShaderProgram> shader_program) 
	:
		ThrowableTile(pos,
			tilemap,
			tilemap_pos,
			shader_program,
			"images/Blocks2.png",
			{ 0.125f, 0.5f }, /* size_in_texture */
			{ 0.0f, 0.5f }, /* pos_in_texture */
			false /* destroyed_on_impact */
		)
{
	m_original_pos = pos;
}

void Barrel::collideWithEntity(Collision collision) 
{
	ThrowableTile::collideWithEntity(collision);
}

void Barrel::setEnabled(bool enabled)
{
	Entity::setEnabled(enabled);

	if (enabled) 
	{
		m_vel = { 0.0f, 0.0f };
		m_thrown = false;
	}
}