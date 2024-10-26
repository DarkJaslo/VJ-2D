#include "ThrowableTile.h"
#include "TimedEvent.h"

#include <iostream>

ThrowableTile::ThrowableTile(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	glm::ivec2 tilemap_pos,
	std::shared_ptr<ShaderProgram> shader_program,
	std::string&& texture_path,
	glm::vec2 size_in_texture,
	glm::vec2 position_in_texture,
	bool destroyed_on_impact) 
{
	m_tilemap = tilemap;
	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile(texture_path, TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(
		{ tilemap->getTileSize(), tilemap->getTileSize() }, /* quad_size */
		size_in_texture,
		m_spritesheet, 
		shader_program));
	m_sprite->setTextureCoordsOffset(position_in_texture);

	m_collision_box_size = { tilemap->getTileSize(), tilemap->getTileSize() };
	setPosition(pos);
	m_destroyed_on_impact = destroyed_on_impact;

	// Idle "animation" and destroy animation
	m_sprite->setNumberAnimations(2);

	m_sprite->setAnimationSpeed(0, 1);
	m_sprite->addKeyframe(0, position_in_texture / size_in_texture);
	
	m_sprite->setAnimationSpeed(1, 6);
	m_sprite->addKeyframe(1, { 7.0f, 0.0f });
	m_sprite->addKeyframe(1, { 7.0f, 1.0f });

	m_sprite->changeAnimation(0);
}

void ThrowableTile::update(int delta_time)
{
	if (!m_enabled)
		return;

	Entity::update(delta_time);
}

void ThrowableTile::collideWithEntity(Collision collision) 
{
	if (!m_destroyed_on_impact)
		return;

	switch (collision.entity->getType())
	{
	case EntityType::Enemy:
	case EntityType::Boss:
	case EntityType::Platform:
	case EntityType::ThrowableTile:
		onDestroy();
		break;
	default:
		break;
	}
}

void ThrowableTile::onPickUp() 
{
	m_can_collide = false;
	m_picked_up = true;
	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
}

void ThrowableTile::onThrow(bool looking_right, bool moving) 
{
	m_can_collide = true;
	m_picked_up = false;
	m_thrown = true;
	m_affected_by_gravity = true;
	m_affected_by_x_drag = true;
	
	if (m_destroyed_on_impact || moving)
	{
		if (looking_right)
			changeVelocity(glm::vec2(1.5f, -0.6f));
		else
			changeVelocity(glm::vec2(-1.5f, -0.6f));
	}
}

void ThrowableTile::onDestroy()
{
	m_can_collide = false;
	m_affected_by_gravity = false;
	setVelocity({ 0.f, 0.f });
	m_sprite->changeAnimation(1);

	auto Destroy = [this]() 
	{
		m_enabled = false;
		m_sprite->changeAnimation(0);
	};
	TimedEvents::pushEvent(std::make_unique<TimedEvent>(350, Destroy));
}