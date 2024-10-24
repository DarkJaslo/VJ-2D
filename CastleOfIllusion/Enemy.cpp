#include "Enemy.h"
#include "Player.h"
#include "TimedEvent.h"

Enemy::Enemy(glm::ivec2 pos,
	glm::ivec2 quad_size,
	std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program,
	std::string&& texture_path,
	glm::vec2 size_in_texture,
	std::shared_ptr<Camera> camera,
	std::shared_ptr<Player> player)
{
	m_camera = camera;
	m_player = player;
	m_tilemap = tilemap;
	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile(texture_path, TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(
		quad_size,
		size_in_texture,
		m_spritesheet,
		shader_program));

	m_collision_box_size = quad_size;
	setPosition(pos);
	m_original_pos = pos;
	
	m_affected_by_x_drag = true;
	m_grounded = true;
}

void Enemy::update(int delta_time) 
{
	m_was_visible = m_is_visible;
	m_is_visible = m_camera->isVisible(m_pos, m_collision_box_size);

	if (!m_is_visible)
	{
		if (!m_camera->isVisible(m_original_pos, m_collision_box_size)) 
			m_pos = m_original_pos;

		if (m_was_visible) 
			disable();
	}
	else
	{
		if (!m_was_visible)
			enable();
	}

	Entity::update(delta_time);
}

void Enemy::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
	{
	case EntityType::Player: 
	{
		auto player = static_cast<Player*>(collision.entity);
		if (player->isAttacking())
		{
			onDeath();
		}
		break;
	}
	case EntityType::ThrowableTile:
	{
		auto throwable = static_cast<ThrowableTile*>(collision.entity);
		if (throwable->isBeingThrown())
			onDeath();
		break;
	}
	default:
		break;
	}
}

void Enemy::enable() 
{
	setEnabled(true);
	m_pos = m_original_pos;
	m_can_collide = true;
	m_vel = { 0.0f, 0.0f };
}

void Enemy::disable() 
{
	setEnabled(false);
	m_pos = { 42000.0f, 42000.0f };
	m_vel = { 0.0f, 0.0f };
}

///////////////// SpringHorse //////////////////////

SpringHorse::SpringHorse(glm::ivec2 pos,
	std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program,
	std::string&& texture_path,
	glm::vec2 size_in_texture,
	std::shared_ptr<Camera> camera,
	std::shared_ptr<Player> player)
	:
		Enemy(pos, 
			{ 64, 128 },
			tilemap, 
			shader_program, 
			std::move(texture_path), 
			size_in_texture,
			camera,
			player)
{
	m_affected_by_gravity = true;

	// Add animations
	m_sprite->setNumberAnimations(4);

	m_sprite->setAnimationSpeed(DEATH, 1);
	m_sprite->addKeyframe(DEATH, { 3.0f, 0.0f });

	m_sprite->setAnimationSpeed(PREPARE_JUMP, 10);
	m_sprite->addKeyframe(PREPARE_JUMP, { 0.0f, 0.0f });
	m_sprite->addKeyframe(PREPARE_JUMP, { 1.0f, 0.0f });
	m_sprite->addKeyframe(PREPARE_JUMP, { 2.0f, 0.0f });
	m_sprite->addKeyframe(PREPARE_JUMP, { 1.0f, 0.0f });
	m_sprite->addKeyframe(PREPARE_JUMP, { 0.0f, 0.0f });

	m_sprite->setAnimationSpeed(JUMPING, 1);
	m_sprite->addKeyframe(JUMPING, { 1.0f, 0.0f });

	m_sprite->setAnimationSpeed(FALLING, 1);
	m_sprite->addKeyframe(FALLING, { 2.0f, 0.0f });

	m_sprite->changeAnimation(PREPARE_JUMP);
	m_sprite->turnLeft();
}

void SpringHorse::update(int delta_time)
{
	Enemy::update(delta_time);

	// Enter start jumping state
	if (m_grounded && !m_jumping) 
	{
		m_jumping = true;
		m_sprite->changeAnimation(PREPARE_JUMP);

		auto Jump = [this]() 
		{
			if (!m_grounded)
				return;

			m_grounded = false;
			m_jumping = false;
			m_vel.y = -1.25f;
			bool look_left = m_player->getPosition().x < m_pos.x;
			m_vel.x = look_left ? -1.25f : 1.25f;
			look_left ? m_sprite->turnLeft() : m_sprite->turnRight();
			m_sprite->changeAnimation(JUMPING);
		};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(500, Jump));

		auto StillJumping = [this]()
		{
			if (!m_grounded)
				m_sprite->changeAnimation(FALLING);
		};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(1000, StillJumping));
	}

	// If jumping, schedule change to fall animation
}

void SpringHorse::enable() 
{
	Enemy::enable();

	m_sprite->changeAnimation(PREPARE_JUMP);
}

void SpringHorse::disable() 
{
	Enemy::disable();
	m_jumping = false;
}

void SpringHorse::onDeath() 
{
	m_sprite->changeAnimation(DEATH);
	m_vel.y = -1.2f;
	m_can_collide = false;
	m_grounded = false;

	auto Disappear = [this]()
	{
		disable();
		std::cout << "disappear" << std::endl;
	};
	//TimedEvents::pushEvent(std::make_unique<TimedEvent>(1000, Disappear));
}