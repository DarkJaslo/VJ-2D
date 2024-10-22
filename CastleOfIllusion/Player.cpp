#include <cmath>
#include <iostream>
#include <memory>
#include <algorithm>

#include <GL/glew.h>
#include "Player.h"
#include "Game.h"
#include "Coin.h"
#include "Cake.h"
#include "ThrowableTile.h"

#define JUMP_HEIGHT 4*16*4
#define MAX_X_VELOCITY 0.6f // Maximum velocity on the x axis
#define MAX_FALL_VELOCITY 0.8f // Maximum velocity when falling
#define ACCELERATION 0.001f // Acceleration on the x axis applied when pressing a key down
#define DECELERATION 0.001f // Deceleration on the x axis applied when not pressing a key down

enum PlayerAnims
{
	IDLE, MOVE, CROUCH, JUMP, FALL,	SLIDE
};

Player::Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, glm::ivec2 const& tilemap_pos, 
			   glm::ivec2 const& sprite_size, glm::ivec2 const& collision_box_size,
	           std::shared_ptr<ShaderProgram> shader_program)
{ 
	std::cout << "Creating player at position " << pos.x << "," << pos.y << std::endl;

	m_tilemap = tilemap;
	m_collision_box_size = collision_box_size;
	m_spritesheet.reset(new Texture());
	m_grounded = false;
	m_vel = glm::vec2(0.f,0.f);
	m_acc = glm::vec2(0.f,0.f);

	m_spritesheet->loadFromFile("images/MickeyMouse.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_sprite.reset(Sprite::createSprite(sprite_size, glm::vec2(0.0625, 0.09090909), m_spritesheet, shader_program));
	
	setPosition(pos);
	configureAnimations();

	m_sprite->changeAnimation(0);
	m_tilemap_displ = tilemap_pos;
	//m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::update(int delta_time)
{
	m_sprite->update(delta_time);
	PlayerState prev_state = m_state;

	// X AXIS
	if (Game::getKey(GLFW_KEY_A))
	{
		m_sprite->turnLeft();
		if (!Game::getKey(GLFW_KEY_S))
		{
			if (m_grounded && !Game::getKey(GLFW_KEY_S))
			{
				m_state = PlayerState::Moving;
			}
			m_acc.x = -ACCELERATION;
		}
	}
	else if (Game::getKey(GLFW_KEY_D))
	{
		m_sprite->turnRight();
		if (!Game::getKey(GLFW_KEY_S))
		{
			if (m_grounded)
			{
				m_state = PlayerState::Moving;
			}
			m_acc.x = ACCELERATION;
		}
	}
	else
	{
		if (m_grounded)
		{
			if (abs(m_vel.x) > 0)
			{
				m_state = PlayerState::Sliding;
			}
			else
			{
				m_state = PlayerState::Idle;
			}
		}
	}

	if ((!Game::getKey(GLFW_KEY_A) && !Game::getKey(GLFW_KEY_D)) || Game::getKey(GLFW_KEY_S))
	{
		m_acc.x = 0.f;
		if (m_vel.x > 0.f)
		{
			m_vel.x = std::max(m_vel.x - DECELERATION * delta_time, 0.0f);
		}
		else if (m_vel.x < 0.f)
		{
			m_vel.x = std::min(m_vel.x + DECELERATION * delta_time, 0.0f);
		}
	}
	
	if (Game::getKey(GLFW_KEY_S))
	{
		if (m_grounded)
		{
			if (m_state != PlayerState::Crouching)
			{
				m_state = PlayerState::Crouching;
			}
		}
		else
		{
			if (m_state != PlayerState::Attacking)
			{
				m_state = PlayerState::Attacking;
			}
		}
	}

	// Update x position
	float new_vel = m_vel.x + m_acc.x * static_cast<float>(delta_time);
	if (abs(new_vel) < MAX_X_VELOCITY)
	{
		m_vel.x = new_vel;
	}
	m_pos.x += m_vel.x*static_cast<float>(delta_time);
	
	auto collision = m_tilemap->xCollision(getMinMaxCollisionCoords().first, m_collision_box_size, m_vel);
	if (collision)
	{
		m_pos.x = collision->x + m_collision_box_size.x/2;
	}

	// Y AXIS
	if (m_grounded)
	{
		if (Game::getKey(GLFW_KEY_W))
		{
			if (m_state != PlayerState::Jumping)
			{
				m_state = PlayerState::Jumping;
			}
			m_grounded = false;
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
	
	collision = m_tilemap->yCollision(getMinMaxCollisionCoords().first, m_collision_box_size, m_vel);
	if (collision)
	{
		m_pos.y = collision->y + m_collision_box_size.y;
		m_acc.y = 0.f;
		m_vel.y = 0.f;
	}

	m_grounded = m_tilemap->isGrounded(getMinMaxCollisionCoords().first, m_collision_box_size);
	
	// Change animation if necessary

	if (prev_state != m_state)
	{
		switch (m_state)
		{
			case PlayerState::Idle:
				m_sprite->changeAnimation(IDLE);
				std::cout << "idle" << std::endl;
				break;
			case PlayerState::Moving:
				std::cout << "move" << std::endl;
				m_sprite->changeAnimation(MOVE);
				break;
			case PlayerState::Crouching:
				std::cout << "crouch" << std::endl;
				m_sprite->changeAnimation(CROUCH);
				break;
			case PlayerState::Jumping:
				std::cout << "jump" << std::endl;
				m_sprite->changeAnimation(JUMP);
				break;
			case PlayerState::Sliding:
				std::cout << "slide" << std::endl;
				m_sprite->changeAnimation(SLIDE);
				break;
			case PlayerState::Attacking:
				std::cout << "attack" << std::endl;
				// change animation
				break;
		}
	}

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
		auto cake = static_cast<Cake*>(collision.entity);
		gainPower(cake->getPower());
		return;
	}
	case EntityType::Coin: 
	{
		auto coin = static_cast<Coin*>(collision.entity);
		gainPoints(coin->getPoints());
		return;
	}
	case EntityType::ThrowableTile: 
	{
		auto throwable = static_cast<ThrowableTile*>(collision.entity);
		if (throwable->isStatic()) 
		{
			// If I'm attacking, bounce on it
			if (m_state == PlayerState::Attacking && throwable->isDestroyedOnImpact()) 
			{
				// Bounce up
				m_vel.y = S_BOUNCE_SPEED;
			}
			else 
			{
				// Physically collide with it: in this case it's a bit more difficult because we already have the
				// updated X,Y coordinates of the player instead of being able to update them one by one.
				//
				// However, if we assume certain things about the game, there is a solution that works

				auto [min, max] = throwable->getMinMaxCollisionCoords();

				// If the amount of the player that is inside the box exceeds these values, then the player is 
				// actually not inside in that axis
				int const Y_MAX_MARGIN = m_collision_box_size.y / 2;
				int const X_MAX_MARGIN = m_collision_box_size.x / 4;

				auto player_max_y = m_pos.y;
				auto player_max_x = m_pos.x + m_collision_box_size.x / 2.0f;
				auto player_min_x = m_pos.x - m_collision_box_size.x / 2.0f;

				// We don't contemplate collisions from below against this kind of object
				int y_inside = player_max_y - min.y;
				int x_inside_left = player_max_x - min.x;
				int x_inside_right = max.x - player_min_x + 1;

				bool actually_inside_y = y_inside < Y_MAX_MARGIN;
				bool actually_inside_x = x_inside_left < X_MAX_MARGIN || x_inside_right < X_MAX_MARGIN;

				auto CorrectX = [&]() 
				{
					if (x_inside_left < X_MAX_MARGIN)
						m_pos.x -= x_inside_left;
					else
						m_pos.x += x_inside_right;

					m_vel.x = 0.0f;
				};

				auto CorrectY = [&]() 
				{
					m_pos.y -= y_inside;
					m_vel.y = 0.0f;
					m_grounded = true;
				};

				if (actually_inside_x && actually_inside_y) 
				{
					// Inside in both axes, correct the smallest ("less inside")
					int x_inside = x_inside_left < X_MAX_MARGIN ? x_inside_left : x_inside_right;
					if (y_inside < x_inside) 
						CorrectY();
					else 
						CorrectX();
				}
				else if (actually_inside_y)
					CorrectY();
				else if (actually_inside_x)
					CorrectX();
			}
		}
		return;
	}
	case EntityType::Void:
		onFallOff();
		return;
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

void Player::gainPower(unsigned int gain) 
{
	m_power = std::min(static_cast<unsigned int>(m_max_power), m_power + gain);
	// Update UI, play sound

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

PlayerState Player::getPlayerState() const
{
	return m_state;
}

bool Player::isAttacking() const
{
	return m_state == PlayerState::Attacking;
}

void Player::onFallOff() 
{
	std::cout << "Collided with void! Dying..." << std::endl;
}

void Player::configureAnimations()
{
	m_sprite->setNumberAnimations(6);

	// Idle
	m_sprite->setAnimationSpeed(IDLE, 3);
	m_sprite->addKeyframe(IDLE, glm::vec2(0.f, 1.f));
	m_sprite->addKeyframe(IDLE, glm::vec2(1.f, 1.f));

	// Moving
	// The core animation is only six frames, but the original game makes Mickey blink every 2 loops, and so do we
	m_sprite->setAnimationSpeed(MOVE, 12);

	m_sprite->addKeyframe(MOVE, glm::vec2(2.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(3.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(4.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(5.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(6.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(7.f, 0.f));

	m_sprite->addKeyframe(MOVE, glm::vec2(2.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(3.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(4.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(5.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(6.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(7.f, 0.f));

	m_sprite->addKeyframe(MOVE, glm::vec2(9.f, 0.f)); // blink
	m_sprite->addKeyframe(MOVE, glm::vec2(6.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(7.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(8.f, 0.f)); // blink
	m_sprite->addKeyframe(MOVE, glm::vec2(3.f, 0.f));
	m_sprite->addKeyframe(MOVE, glm::vec2(4.f, 0.f));

	// Crouching
	m_sprite->setAnimationSpeed(CROUCH, 8);
	m_sprite->addKeyframe(CROUCH, glm::vec2(0.f, 2.f));
	m_sprite->addKeyframe(CROUCH, glm::vec2(1.f, 2.f));

	// Jumping
	m_sprite->setAnimationSpeed(JUMP, 1);
	m_sprite->addKeyframe(JUMP, glm::vec2(2.f, 2.f));

	// Falling
	m_sprite->setAnimationSpeed(FALL, 1);
	m_sprite->addKeyframe(FALL, glm::vec2(2.f, 3.f));

	// Sliding
	m_sprite->setAnimationSpeed(SLIDE, 1);
	m_sprite->addKeyframe(SLIDE, glm::vec2(10.f, 0.f));
}