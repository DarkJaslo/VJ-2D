#include "Rock.h"
#include "Player.h"

Rock::Rock(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program) 
	:
		ThrowableTile(pos,
			tilemap,
			{0, 0},
			shader_program,
			"images/Blocks2.png",
			{ 0.125f, 0.5f }, /* size_in_texture */
			{ 0.25f, 0.0f }, /* pos_in_texture */
			true /* destroyed_on_impact */
		)
{
	m_start_pos = pos;

	m_thrown = true;
	m_affected_by_x_drag = false;
	m_bounces = false;

	m_affected_by_gravity = true;
	m_enabled = false;

	M_MAX_FALL_VELOCITY = 0.5f;
}

void Rock::collideWithEntity(Collision collision) 
{
	if (collision.entity->getType() == EntityType::Player) 
	{
		// Destroy and damage player
		auto player = static_cast<Player*>(collision.entity);
		player->takeHit();
		onDestroy();
	}
	else
		ThrowableTile::collideWithEntity(collision);
}

void Rock::setEnabled(bool enabled) 
{
	ThrowableTile::setEnabled(enabled);

	m_affected_by_gravity = enabled;
	if (enabled)
	{
		m_pos = m_start_pos;
		m_can_collide = true;
		m_can_collide_with_tiles = true;
		m_thrown = true;
	}
}