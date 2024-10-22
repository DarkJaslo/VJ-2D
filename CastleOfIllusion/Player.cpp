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

#define SIZE_IN_TEXTURE_X 0.0625
#define SIZE_IN_TEXTURE_Y 0.09090909

#define KEY_MOVE_LEFT GLFW_KEY_A
#define KEY_MOVE_RIGHT GLFW_KEY_D
#define KEY_MOVE_DOWN GLFW_KEY_S
#define KEY_JUMP GLFW_KEY_W
#define KEY_GRAB GLFW_KEY_K

enum PlayerAnims
{
	IDLE, MOVE, CROUCH, JUMP, FALL,	SLIDE, ATTACK_UP, ATTACK_DOWN,
	HOLD_IDLE, HOLD_MOVE, HOLD_JUMP, HOLD_FALL,
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
	m_throwable_obj = NULL;
	m_has_object = false;
	m_looking_right = true;

	m_spritesheet->loadFromFile("images/MickeyMouse.png", TEXTURE_PIXEL_FORMAT_RGBA);

	m_sprite.reset(Sprite::createSprite(sprite_size, glm::vec2(SIZE_IN_TEXTURE_X, SIZE_IN_TEXTURE_Y), m_spritesheet, shader_program));
	
	setPosition(pos);
	configureAnimations();

	m_sprite->changeAnimation(0);
	m_tilemap_displ = tilemap_pos;

	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
}

void Player::update(int delta_time)
{
	m_sprite->update(delta_time);
	PlayerState prev_state = m_state;


	// Handle grab interaction
	if (Game::getKey(KEY_GRAB) && (m_throwable_obj != NULL))
	{
		m_has_object = !m_has_object;
		if (m_has_object)
		{
			m_throwable_obj->onPickUp();
		}
		else
		{
			m_throwable_obj->onThrow(m_vel);
			m_throwable_obj = NULL;
		}
	}

	//////// X AXIS
	if (Game::getKey(KEY_MOVE_LEFT))
	{
		m_sprite->turnLeft();
		m_looking_right = false;

		// Don't move if crouching
		if (!(m_state == PlayerState::Crouching))
		{
			if (m_grounded)
			{
				if (m_has_object)
				{
					m_state = PlayerState::HoldMoving;
				}
				else
				{
					m_state = PlayerState::Moving;
				}
			}
			m_acc.x = -ACCELERATION;
		}
	}
	else if (Game::getKey(KEY_MOVE_RIGHT))
	{
		m_sprite->turnRight();
		m_looking_right = true;

		//Don't move if crouching
		if (!(m_state == PlayerState::Crouching))
		{
			if (m_grounded)
			{
				if (m_has_object)
				{
					m_state = PlayerState::HoldMoving;
				}
				else
				{
					m_state = PlayerState::Moving;
				}
			}
			m_acc.x = ACCELERATION;
		}
	}
	else
	{
		if (m_grounded)
		{
			if (m_has_object)
			{
				if (abs(m_vel.x) > 0)
				{
					m_state = PlayerState::HoldMoving;
				}
				else
				{
					m_state = PlayerState::HoldIdle;
				}
			}
			else
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
	}

	// Decelerate movement on the x axis if the player has stopped moving or is crouching
	if ((!Game::getKey(KEY_MOVE_LEFT) && !Game::getKey(KEY_MOVE_RIGHT)) || (m_state == PlayerState::Crouching))
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
	
	// Crouch/attack
	// The player can't do neither of those actions while holding an object
	if (Game::getKey(KEY_MOVE_DOWN) && !m_has_object)
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
			if (m_vel.y < 0)
			{
				if (m_state != PlayerState::AttackingUp)
				{
					m_state = PlayerState::AttackingUp;
				}
			}
			else
			{
				if (m_state != PlayerState::AttackingDown)
				{
					m_state = PlayerState::AttackingDown;
				}
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

	//////// Y AXIS

	if (m_grounded)
	{
		// Jump
		if (Game::getKey(KEY_JUMP))
		{
			if (m_has_object && m_state != PlayerState::HoldJumping)
			{
				m_state = PlayerState::HoldJumping;
			}
			else if (m_state != PlayerState::Jumping)
			{
				m_state = PlayerState::Jumping;
			}
			m_grounded = false;
			m_vel.y = calculateJumpVelocity(JUMP_HEIGHT, S_GRAVITY);
		}
	}
	else
	{
		// Update from going up to going down
		if (m_vel.y > 0)
		{
			if (m_has_object)
			{
				if (m_state == PlayerState::HoldJumping)
				{
					m_state = PlayerState::HoldFalling;
				}
			}
			else {
				if (m_state == PlayerState::AttackingUp)
				{
					m_state = PlayerState::AttackingDown;
				}
				else if (m_state != PlayerState::Falling && m_state != PlayerState::AttackingDown)
				{
					m_state = PlayerState::Falling;
				}
			}
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


	// Update grounded
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
			case PlayerState::Falling:
				std::cout << "jump" << std::endl;
				m_sprite->changeAnimation(FALL);
				break;
			case PlayerState::Sliding:
				std::cout << "slide" << std::endl;
				m_sprite->changeAnimation(SLIDE);
				break;
			case PlayerState::AttackingUp:
				std::cout << "attack up" << std::endl;
				m_sprite->changeAnimation(ATTACK_UP);
				break;
			case PlayerState::AttackingDown:
				std::cout << "attack down" << std::endl;
				m_sprite->changeAnimation(ATTACK_DOWN);
				break;
			case PlayerState::HoldIdle:
				std::cout << "hold idle" << std::endl;
				m_sprite->changeAnimation(HOLD_IDLE);
				break;
			case PlayerState::HoldMoving:
				std::cout << "hold move" << std::endl;
				m_sprite->changeAnimation(HOLD_MOVE);
				break;
			case PlayerState::HoldJumping:
				std::cout << "hold jump" << std::endl;
				m_sprite->changeAnimation(HOLD_JUMP);
				break;
			case PlayerState::HoldFalling:
				std::cout << "hold fall" << std::endl;
				m_sprite->changeAnimation(HOLD_FALL);
				break;
		}
	}

	// Update sprite position
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));
	
	// Update holding object position
	if (m_has_object)
	{
		m_throwable_obj->setPosition(m_pos + glm::ivec2(0.f,-24.f*4));
	}
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
			if (isAttacking() && throwable->isDestroyedOnImpact())
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
	return m_state == PlayerState::AttackingDown;
}

void Player::onFallOff() 
{
	std::cout << "Collided with void! Dying..." << std::endl;
}

void Player::configureAnimations()
{
	m_sprite->setNumberAnimations(12);

	// Idle
	m_sprite->setAnimationSpeed(IDLE, 3);
	m_sprite->addKeyframe(IDLE, glm::vec2(0.f, 0.f));
	m_sprite->addKeyframe(IDLE, glm::vec2(1.f, 0.f));

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
	m_sprite->addKeyframe(CROUCH, glm::vec2(0.f, 1.f));
	m_sprite->addKeyframe(CROUCH, glm::vec2(1.f, 1.f));

	// Jumping
	m_sprite->setAnimationSpeed(JUMP, 1);
	m_sprite->addKeyframe(JUMP, glm::vec2(2.f, 1.f));

	// Falling
	m_sprite->setAnimationSpeed(FALL, 1);
	m_sprite->addKeyframe(FALL, glm::vec2(3.f, 1.f));

	// Sliding
	m_sprite->setAnimationSpeed(SLIDE, 1);
	m_sprite->addKeyframe(SLIDE, glm::vec2(10.f, 0.f));

	// Attacking up
	m_sprite->setAnimationSpeed(ATTACK_UP, 1);
	m_sprite->addKeyframe(ATTACK_UP, glm::vec2(5.f, 1.f));

	// Attacking down
	m_sprite->setAnimationSpeed(ATTACK_DOWN, 1);
	m_sprite->addKeyframe(ATTACK_DOWN, glm::vec2(6.f, 1.f));

	// Holding idle
	m_sprite->setAnimationSpeed(HOLD_IDLE, 3);
	m_sprite->addKeyframe(HOLD_IDLE, glm::vec2(11.f, 1.f));
	m_sprite->addKeyframe(HOLD_IDLE, glm::vec2(12.f, 1.f));

	// Moving while holdinbg an object
	// The core animation is only six frames, but the original game makes Mickey blink every 2 loops, and so do we
	m_sprite->setAnimationSpeed(HOLD_MOVE, 12);

	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(0.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(1.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(2.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(3.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(4.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(5.f, 2.f));

	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(0.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(1.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(2.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(3.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(4.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(5.f, 2.f));

	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(7.f, 2.f)); // blink
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(4.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(5.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(6.f, 2.f)); // blink
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(1.f, 2.f));
	m_sprite->addKeyframe(HOLD_MOVE, glm::vec2(2.f, 2.f));

	// Jumping
	m_sprite->setAnimationSpeed(HOLD_JUMP, 1);
	m_sprite->addKeyframe(HOLD_JUMP, glm::vec2(8.f, 2.f));

	// Falling
	m_sprite->setAnimationSpeed(HOLD_FALL, 1);
	m_sprite->addKeyframe(HOLD_FALL, glm::vec2(9.f, 2.f));
}