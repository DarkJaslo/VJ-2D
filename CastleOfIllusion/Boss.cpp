#include "Boss.h"
#include "TimedEvent.h"

#include <algorithm>
#include <random>
#include <iostream>

int Boss::S_FACE_OFFSET;

Boss::Boss(glm::ivec2 spawn_pos,
	glm::ivec2 other_pos,
	glm::ivec2 quad_size,
	std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program,
	std::string const& boss_texture_path,
	std::string const& blocks_texture_path,
	std::shared_ptr<Camera> camera)
{
	//m_enabled = false;

	m_pos = spawn_pos;
	m_first_pos = spawn_pos;
	m_other_pos = other_pos;
	m_tilemap = tilemap;
	m_collision_box_size = { m_tilemap->getTileSize() * 3, m_tilemap->getTileSize() * 3 };
	S_FACE_OFFSET = -m_tilemap->getTileSize() / 2;

	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile(boss_texture_path, TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(
		quad_size,
		{ 0.3333333f, 1.0f },
		m_spritesheet,
		shader_program));

	// Add animations
	m_sprite->setNumberAnimations(Animation::Count);

	// Default
	m_sprite->setAnimationSpeed(0, 1);
	m_sprite->addKeyframe(Animation::Default, {0.0f, 0.0f});

	// Taking damage
	m_sprite->setAnimationSpeed(Animation::TakingDamage, 8);
	m_sprite->addKeyframe(Animation::TakingDamage, { 2.0f, 0.0f });
	m_sprite->addKeyframe(Animation::TakingDamage, { 1.0f, 0.0f });

	m_sprite->changeAnimation(Animation::Default);

	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
	m_can_collide = true;
	m_grounded = true;

	// Initialize block offsets
	m_block_offsets.resize(9);
	m_block_offsets[0] = { -1, -2 };
	m_block_offsets[1] = { 0, -2 };
	m_block_offsets[2] = { 1, -2 };
	m_block_offsets[3] = { -1, -1 };
	m_block_offsets[4] = { 0, -1 };
	m_block_offsets[5] = { 1, -1 };
	m_block_offsets[6] = { -1,  0 };
	m_block_offsets[7] = { 0,  0 };
	m_block_offsets[8] = { 1,  0 };
	for (auto& offset : m_block_offsets)
		offset *= m_tilemap->getTileSize();

	m_blocks.resize(9);
	for (int i = 0; i < m_blocks.size(); ++i)
		initBlock(i);

	std::shared_ptr<Texture> face_tex;
	face_tex.reset(new Texture());
	face_tex->loadFromFile(blocks_texture_path, TEXTURE_PIXEL_FORMAT_RGBA);
	m_miniface.reset(Sprite::createSprite(
		{ m_tilemap->getTileSize(), m_tilemap->getTileSize() },
		{ 0.125f, 0.5f },
		face_tex,
		shader_program));
	m_miniface->setTextureCoordsOffset({ 0.75f, 0.0f });
}

void Boss::update(int delta_time) 
{
	if (!m_enabled)
		return;

	auto pre_pos = m_pos;

	Entity::update(delta_time);
	m_sprite->setPosition(m_pos + glm::ivec2(0, S_FACE_OFFSET));


	for (auto& block : m_blocks) 
	{
		if (m_state != State::Move && pre_pos != m_pos) 
		{
			auto change = m_pos - pre_pos;
			block->changePosition(change);
		}
	}

	switch (m_state) 
	{
	case State::Vulnerable: 
	{
		if (!m_sent_event) 
		{
			m_sent_event = true;
			auto Jump = [this]() 
			{
				if (m_state == State::TakeHit || m_state == State::Dying)
					return;

				m_state = State::Jump;
				m_sent_event = false;

				m_affected_by_gravity = true;
				m_vel.y = -0.75f;
				m_grounded = false;
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(2000, Jump));
		}
		break;
	}
	case State::Jump: 
	{
		if (m_grounded && !m_sent_event) 
		{
			m_sent_event = true;

			auto ChangeToWait = [this]() 
			{
				if (m_state == State::TakeHit || m_state == State::Dying)
					return;

				// Make stuff vibrate
				m_state = State::Wait;
				m_sent_event = false;

				m_affected_by_gravity = false;
				m_vel.y = 0.0f;
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(300, ChangeToWait));
		}
		break;
	}
	case State::Wait: 
	{
		if (!m_sent_event) 
		{
			m_sent_event = true;

			auto Wait = [this]() 
			{
				if (m_state == State::TakeHit || m_state == State::Dying)
					return;

				m_state = State::Move;
				m_sent_event = false;
				m_vulnerable = false;
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(1000, Wait));
		}
		break;
	}
	case State::Move: 
	{
		if (!m_sent_event) 
		{
			for (auto& block : m_blocks) 
				block->setNotMoved();

			m_sent_event = true;

			glm::ivec2 target_pos = m_currently_first_pos ? m_other_pos : m_first_pos;
			m_currently_first_pos = !m_currently_first_pos;

			m_pos = target_pos;

			// Random move order
			std::vector<int> nums = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(nums.begin(), nums.end(), g);

			m_last_to_send = nums.back();
			m_currently_sending = -1;

			auto SendBlock = [nums, target_pos, this]()
			{
				++m_currently_sending;

				int block_index = nums[m_currently_sending];
				m_blocks[block_index]->attachFace(m_miniface);

				auto Send = [block_index, target_pos, this]() 
				{
					glm::ivec2 block_target = target_pos + m_block_offsets[block_index];

					// Schedule the send to last 500ms
					m_blocks[block_index]->move(block_target, 300);
				};
				// Start sending after 400ms
				TimedEvents::pushEvent(std::make_unique<TimedEvent>(400, Send));
			};

			for (int i = 0; i < 9; ++i) 
			{
				int current_time = 800 * (i+1);
				TimedEvents::pushEvent(std::make_unique<TimedEvent>(current_time, SendBlock));
			}

		}
		else 
		{
			if (m_blocks[m_last_to_send]->moved()) 
			{
				m_state = State::Vulnerable;
				m_vulnerable = true;
				m_sent_event = false;
			}
		}
		break;
	}
	case State::Dying:
		break;
	case State::TakeHit:
		break;
	default: 
	{
		throw std::runtime_error("Boss::update: the boss can't be in this state!");
	}
	}
}

void Boss::render() 
{
	if (m_vulnerable)
		m_sprite->render();
}

void Boss::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
	{
	case EntityType::ThrowableTile: 
	{
		auto tile = static_cast<ThrowableTile*>(collision.entity);
		if (m_vulnerable && tile->isBeingThrown())
			takeHit();
		break;
	}
	default:
		break;
	}
}


void Boss::setPosition(glm::ivec2 new_position) 
{
	m_pos = new_position;
	m_sprite->setPosition(m_pos + glm::ivec2( 0, S_FACE_OFFSET ));
}

void Boss::changePosition(glm::ivec2 change)
{
	m_pos += change;
	m_sprite->setPosition(m_pos + glm::ivec2(0, S_FACE_OFFSET));
}

void Boss::initBlock(int i) 
{
	auto& block = m_blocks[i];

	block.reset(
		new BossBlock(m_tilemap, m_sprite->getShaderProgram()
	));

	block->setPosition(m_pos + m_block_offsets[i]);
	//block->setEnabled(false);
}

void Boss::takeHit() 
{
	if (--m_life == 0) 
	{
		// Die
		m_state = State::Dying;
		m_can_collide = false;
		m_sprite->changeAnimation(Animation::TakingDamage);

		auto Explode = [this]() 
		{
			m_vulnerable = false;
			m_can_collide = false;
			m_can_collide_with_tiles = false;

			glm::ivec2 center = m_pos + glm::ivec2(0.0f, S_FACE_OFFSET);
			for (auto& block : m_blocks) 
			{
				glm::ivec2 direction = block->getPosition() - center;
				block->enableGravityAndDrag();
				glm::vec2 velocity{ static_cast<float>(direction.x), static_cast<float>(direction.y) };
				velocity *= 0.03;
				block->setVelocity(velocity);
			}

			auto Disable = [this]() 
			{
				setEnabled(false);
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(5000, Disable));
		};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(2000, Explode));
	}
	else 
	{
		m_state = State::TakeHit;

		// Play animation
		m_sprite->changeAnimation(Animation::TakingDamage);

		auto ChangeToVulnerable = [this]()
		{
			m_sent_event = false;
			m_vulnerable = true;
			m_state = State::Vulnerable;
			// Stop animation
			m_sprite->changeAnimation(Animation::Default);
		};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(2000, ChangeToVulnerable));
	}
}










// BossBlock

BossBlock::BossBlock(std::shared_ptr<TileMap> tilemap,
	std::shared_ptr<ShaderProgram> shader_program) 
{
	std::string path = "images/Blocks2.png";

	glm::vec2 size_in_texture{ 0.125f, 0.5f };
	glm::vec2 pos_in_texture{ 0.75f , 0.5f };
	glm::ivec2 quad_size = { tilemap->getTileSize(), tilemap->getTileSize() };

	m_tilemap = tilemap;

	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile(path, TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(
		quad_size,
		size_in_texture,
		m_spritesheet,
		shader_program));
	m_sprite->setTextureCoordsOffset(pos_in_texture);

	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
	m_can_collide = false;
	m_can_collide_with_tiles = false;
	m_enabled = true;
}

void BossBlock::update(int delta_time) 
{
	Entity::update(delta_time);

	if (m_face) 
		m_face->setPosition(m_pos);

	if (m_moving) 
	{
		if ((m_vel.x > 0 && m_pos.x >= m_target_x) || (m_vel.x < 0 && m_pos.x <= m_target_x)) 
		{
			m_moving = false;
			m_moved = true;
			m_can_collide = false;
			m_vel.x = 0.0f;
			m_pos.x = m_target_x;
			detachFace();
		}
	}
}

void BossBlock::render() 
{
	m_sprite->render();

	if (m_face) 
	{
		m_face->render();
	}
}

void BossBlock::move(glm::ivec2 target_pos, int target_ms) 
{
	m_moving = true;
	m_can_collide = true;

	m_target_x = target_pos.x;
	
	// We can't take more than target_ms, so we subtract 50 for safety
	target_ms -= 50;

	if (target_ms <= 0)
		throw std::runtime_error("BossBlock::move: negative or zero target ms!");

	float distance = static_cast<float>(target_pos.x - m_pos.x);
	float speed = distance / static_cast<float>(target_ms);

	m_vel.x = speed;
}

void BossBlock::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
	{
	case EntityType::Void: 
	{
		setEnabled(false);
		break;
	}
	default:
		break;
	}
}

void BossBlock::enableGravityAndDrag() 
{
	m_affected_by_gravity = true;
	m_affected_by_x_drag = true;
}