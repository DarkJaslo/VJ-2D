#include <cmath>
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

Player::Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos,
	           std::shared_ptr<ShaderProgram> shader_program)
{ 
	m_tilemap = tilemap;
	m_spritesheet.reset(new Texture());
	m_is_jumping = false;
	m_spritesheet->loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25, 0.25), m_spritesheet, shader_program));
	setPosition(pos);
	m_sprite->setNumberAnimations(4);

	{
		m_sprite->setAnimationSpeed(STAND_LEFT, 8);
		m_sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

		m_sprite->setAnimationSpeed(STAND_RIGHT, 8);
		m_sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

		m_sprite->setAnimationSpeed(MOVE_LEFT, 8);
		m_sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
		m_sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
		m_sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));

		m_sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		m_sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
		m_sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.25f));
		m_sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.5f));
	}

	m_sprite->changeAnimation(0);
	m_tilemap_displ = tilemap_pos;
	m_sprite->setPosition(glm::vec2(float(m_tilemap_displ.x + m_pos.x), float(m_tilemap_displ.y + m_pos.y)));
}

void Player::update(int delta_time)
{
	m_sprite->update(delta_time);
	if (Game::getKey(GLFW_KEY_LEFT))
	{
		if (m_sprite->animation() != MOVE_LEFT)
			m_sprite->changeAnimation(MOVE_LEFT);
		m_pos.x -= 2;
		if (m_tilemap->collisionMoveLeft(m_pos, glm::ivec2(32, 32)))
		{
			m_pos.x += 2;
			m_sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::getKey(GLFW_KEY_RIGHT))
	{
		if (m_sprite->animation() != MOVE_RIGHT)
			m_sprite->changeAnimation(MOVE_RIGHT);
		m_pos.x += 2;
		if (m_tilemap->collisionMoveRight(m_pos, glm::ivec2(32, 32)))
		{
			m_pos.x -= 2;
			m_sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (m_sprite->animation() == MOVE_LEFT)
			m_sprite->changeAnimation(STAND_LEFT);
		else if (m_sprite->animation() == MOVE_RIGHT)
			m_sprite->changeAnimation(STAND_RIGHT);
	}
	
	if (m_is_jumping)
	{
		m_jump_angle += JUMP_ANGLE_STEP;
		if (m_jump_angle == 180)
		{
			m_is_jumping = false;
			m_pos.y = m_start_y;
		}
		else
		{
			m_pos.y = int(m_start_y - 96 * sin(3.14159f * m_jump_angle / 180.f));
			// mirar aqui si hay colisiones arriba?
			if (m_jump_angle > 90)
				m_is_jumping = !m_tilemap->collisionMoveDown(m_pos, glm::ivec2(32, 32), &m_pos.y);
			else
			{
				m_is_jumping = !m_tilemap->collisionMoveUp(m_pos, glm::ivec2(32, 32), &m_pos.y);
			}
		}
	}
	else
	{
		m_pos.y += FALL_STEP;
		if (m_tilemap->collisionMoveDown(m_pos, glm::ivec2(32, 32), &m_pos.y))
		{
			if (Game::getKey(GLFW_KEY_UP))
			{
				m_is_jumping = true;
				m_jump_angle = 0;
				m_start_y = m_pos.y;
			}
		}
	}
	
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::collideWithEntity(Collision collision) 
{
	switch (collision.type) 
	{
	case EntityType::Enemy:
	case EntityType::Projectile:
	case EntityType::Boss:
	{
		takeHit();
		return;
	}
	case EntityType::Cake: 
	{
		gainPower();
		return;
	}
	case EntityType::Coin: 
	{
		// Some hardcoded number for now
		gainPoints(123);
		return;
	}
	default:
		return;
	}
}

void Player::takeHit() 
{
	// Check vulnerability

	--m_power;

	// Debug
	std::cout << "Power left: " << m_power << "\n";

	if (--m_power <= 0) 
	{
		// Lose game
	}

	// Trigger invulnerability for a while

	// Flicker animation

	// Update UI, play sound
}

void Player::gainPower() 
{
	if (m_power + 1 <= m_max_power) 
	{
		++m_power;
		// Update UI, play sound
	}

	// Debug
	std::cout << "Power: " << m_power << "\n";
}

void Player::gainPoints(unsigned int gain) 
{
	m_points += gain;

	// Update UI
}
