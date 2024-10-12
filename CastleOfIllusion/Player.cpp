#include <cmath>
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"

#define JUMP_HEIGHT 80 
#define MAX_VELOCITY 0.4f // Maximum velocity on the x axis
#define ACCELERATION 0.001f // Acceleration on the x axis applied when pressing a key down
#define DECELERATION 0.002f // Deceleration on the x axis applied when not pressing a k	ey down
#define GRAVITY 0.003f

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

Player::Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos,
	           std::shared_ptr<ShaderProgram> shader_program)
{ 
	m_tilemap = tilemap;
	m_spritesheet.reset(new Texture());
	m_is_grounded = false;
	m_vel = glm::vec2(0.f,0.f);
	m_acc = glm::vec2(0.f,0.f);
	m_spritesheet->loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(glm::vec2(32, 32), glm::vec2(0.25, 0.25), m_spritesheet, shader_program));
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
	float new_vel = m_vel.x + m_acc.x * float(delta_time);
	if (abs(new_vel) < MAX_VELOCITY)
	{
		m_vel.x = new_vel;
	}
	m_pos.x += m_vel.x*float(delta_time);
	
	auto collision = m_tilemap->xCollision(m_pos, glm::ivec2(32, 32), m_vel);
	if (collision)
	{
		m_pos.x = (*collision).x;
	}

	// Y AXIS
	if (m_is_grounded)
	{
		if (Game::getKey(GLFW_KEY_UP))
		{
			m_is_grounded = false;
			m_vel.y = calculateJumpVelocity(JUMP_HEIGHT);
		}
	}
	
	// Update y position
	m_vel.y += GRAVITY * float(delta_time);
	m_pos.y += m_vel.y*float(delta_time);
	
	collision = m_tilemap->yCollision(m_pos, glm::ivec2(32,32), m_vel);
	if (collision)
	{
		m_pos.y = (*collision).y;
		m_acc.y = 0.f;
		m_vel.y = 0.f;
	}

	m_is_grounded = m_tilemap->isGrounded(m_pos, glm::ivec2(32,32));
	
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::updateMovement(int delta_time)
{
	glm::vec2 new_vel = m_vel + m_acc * float(delta_time);
	
	if (abs(new_vel.x) < MAX_VELOCITY)
	{
		m_vel = new_vel;
	}
	
	m_pos += m_vel*float(delta_time);
}

float Player::calculateJumpVelocity(float height)
{
	return -sqrt(2*GRAVITY*height);
}