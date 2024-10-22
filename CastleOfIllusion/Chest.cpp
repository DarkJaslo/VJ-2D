#include "Chest.h"
#include "TimedEvent.h"
#include "Player.h"

Chest::Chest(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	glm::ivec2 tilemap_pos,
	std::shared_ptr<ShaderProgram> shader_program,
	std::shared_ptr<Entity> content)
	:
		ThrowableTile(pos,
			tilemap,
			tilemap_pos,
			shader_program,
			"images/Items.png",
			{ 0.125f, 0.5f }, /* size_in_texture */
			{ 0.75f, 0.0f }, /* pos_in_texture */
			true /* destroyed_on_impact */
		),
		m_content(content)
{
	m_affected_by_x_drag = false;
}

void Chest::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType())
	{
	case EntityType::Player: 
	{
		auto player = static_cast<Player*>(collision.entity);
		if (player->isAttacking())
			onDestroy();
		break;
	}
	default:
		break;
	}
}

void Chest::onDestroy() 
{
	ThrowableTile::onDestroy();

	m_content->setPosition(m_pos);
	m_content->setVelocity({ 0.0f, -2.0f });
	m_content->setEnabled(true);
}