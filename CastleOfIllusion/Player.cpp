#include <cmath>
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"
#include "Coin.h"

#define JUMP_HEIGHT 100
#define MAX_X_VELOCITY 0.4f // Maximum velocity on the x axis
#define MAX_FALL_VELOCITY 0.4f // Maximum velocity when falling
#define ACCELERATION 0.001f // Acceleration on the x axis applied when pressing a key down
#define DECELERATION 0.002f // Deceleration on the x axis applied when not pressing a k	ey down

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

Player::Player(glm::ivec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos,
	           std::shared_ptr<ShaderProgram> shader_program)
{ 
	std::cout << "Creating player at position " << pos.x << "," << pos.y << std::endl;

	m_tilemap = tilemap;
	m_spritesheet.reset(new Texture());
	m_is_grounded = false;
	m_vel = glm::vec2(0.f,0.f);
	m_acc = glm::vec2(0.f,0.f);
	m_collision_box_size = glm::ivec2(32, 32);
	m_spritesheet->loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(glm::vec2(50,50), glm::vec2(0.25, 0.25), m_spritesheet, shader_program));
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
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::update(int delta_time)
{
	m_sprite->update(delta_time);

	// X AXIS
	if (Game::getKey(GLFW_KEY_LEFT))
	{
		if (m_sprite->animation() != MOVE_LEFT)
			m_sprite->changeAnimation(MOVE_LEFT);
		
		m_acc.x = -ACCELERATION;
	}
	else if (Game::getKey(GLFW_KEY_RIGHT))
	{
		if (m_sprite->animation() != MOVE_RIGHT)
			m_sprite->changeAnimation(MOVE_RIGHT);

		m_acc.x = ACCELERATION;
	}
	else
	{
		m_acc.x = 0.f;
		if (m_vel.x > 0.f)
		{
			m_vel.x = std::max(m_vel.x - DECELERATION*delta_time, 0.0f);
		}
		else if (m_vel.x < 0.f)
		{
			m_vel.x = std::min(m_vel.x + DECELERATION*delta_time, 0.0f);
		}
		
		if (m_sprite->animation() == MOVE_LEFT)
			m_sprite->changeAnimation(STAND_LEFT);
		else if (m_sprite->animation() == MOVE_RIGHT)
			m_sprite->changeAnimation(STAND_RIGHT);
	}
	
	// Update x position
	float new_vel = m_vel.x + m_acc.x * static_cast<float>(delta_time);
	if (abs(new_vel) < MAX_X_VELOCITY)
	{
		m_vel.x = new_vel;
	}
	m_pos.x += m_vel.x*static_cast<float>(delta_time);
	
	auto collision = m_tilemap->xCollision(m_pos, glm::ivec2(50, 50), m_vel);
	if (collision)
	{
		m_pos.x = collision->x;
	}

	// Y AXIS
	if (m_is_grounded)
	{
		if (Game::getKey(GLFW_KEY_UP))
		{
			m_is_grounded = false;
			m_vel.y = calculateJumpVelocity(JUMP_HEIGHT, S_GRAVITY);
		}
	}
	
	// Update y position
	new_vel = m_vel.y + S_GRAVITY*static_cast<float>(delta_time);
	if (new_vel < MAX_FALL_VELOCITY)
	{
		m_vel.y = new_vel;
	}
	m_pos.y += m_vel.y*static_cast<float>(delta_time);
	
	collision = m_tilemap->yCollision(m_pos, glm::ivec2(50,50), m_vel);
	if (collision)
	{
		m_pos.y = collision->y;
		m_acc.y = 0.f;
		m_vel.y = 0.f;
	}

	m_is_grounded = m_tilemap->isGrounded(m_pos, glm::ivec2(50,50));
	
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
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
		auto coin = static_cast<Coin*>(collision.entity);
		gainPoints(coin->getPoints());
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

	// Debug
	std::cout << "Points: " << m_points << "\n";
}

float Player::calculateJumpVelocity(float height, float gravity) const
{
	return -sqrt(2*gravity*height);
}
