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
#include "TimedEvent.h"
#include "Enemy.h"
#include "Camera.h"

#define JUMP_HEIGHT 4*16*4
#define MAX_X_VELOCITY 0.6f // Maximum velocity on the x axis
#define MAX_FALL_VELOCITY 0.8f // Maximum velocity when falling
#define ACCELERATION 0.001f // Acceleration on the x axis applied when pressing a key down
#define DECELERATION 0.002f // Deceleration on the x axis applied when not pressing a key down

#define SIZE_IN_TEXTURE_X 0.0625
#define SIZE_IN_TEXTURE_Y 0.09090909

#define KEY_MOVE_LEFT GLFW_KEY_A
#define KEY_MOVE_RIGHT GLFW_KEY_D
#define KEY_MOVE_DOWN GLFW_KEY_S
#define KEY_JUMP GLFW_KEY_W
#define KEY_GRAB GLFW_KEY_K

enum PlayerAnims
{
	IDLE = 0, MOVE, CROUCH, JUMP, FALL,	SLIDE, ATTACK_UP, ATTACK_DOWN,
	HOLD_IDLE, HOLD_MOVE, HOLD_JUMP, HOLD_FALL, HURT,
	COUNT // Always keep this one last
};

Player::Player(glm::vec2 const& pos, std::shared_ptr<TileMap> tilemap, std::shared_ptr<UI> ui,
			   glm::ivec2 const& tilemap_pos, glm::ivec2 const& sprite_size, 
			   glm::ivec2 const& collision_box_size, std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Camera> camera)
{
	m_affected_by_x_drag = false;
	m_affected_by_gravity = true;
	M_MAX_FALL_VELOCITY = MAX_FALL_VELOCITY;
	M_MAX_X_VELOCITY = MAX_X_VELOCITY;

	m_original_pos = pos;
	m_tilemap = tilemap;
	m_ui = ui;
	m_collision_box_size = collision_box_size;
	m_spritesheet.reset(new Texture());
	m_grounded = false;
	m_vel = glm::vec2(0.f,0.f);
	m_acc = glm::vec2(0.f,0.f);
	m_throwable_obj = nullptr;
	m_power = m_max_power;
	m_camera = camera;

	m_ui->setPower(m_max_power);
	m_ui->setTries(m_tries);

	m_spritesheet->loadFromFile("images/MickeyMouse.png", TEXTURE_PIXEL_FORMAT_RGBA);

	m_sprite.reset(Sprite::createSprite(sprite_size, glm::vec2(SIZE_IN_TEXTURE_X, SIZE_IN_TEXTURE_Y), m_spritesheet, shader_program));
	
	setPosition(pos);
	configureAnimations();

	m_sprite->changeAnimation(0);
	m_tilemap_displ = tilemap_pos;

	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));

	m_power = 3;
}

void Player::update(int delta_time)
{
	PlayerState prev_state = m_state;

	// Check if hacks have been used
	if (Game::getKey(GLFW_KEY_H))
	{
		gainPower(m_max_power);
	}
	if (Game::getKey(GLFW_KEY_G))
	{
		Game::keyReleased(GLFW_KEY_G);
		m_god_mode = !m_god_mode;
	}

	// The hurt state has priority over all the others
	// It has to be updated here and not on takeHit(), otherwise the states 
	// and animations don't update correctly
	if (m_hurt)
		m_state = PlayerState::Hurt;
	if (!m_hurt && m_state == PlayerState::Hurt)
		m_state = PlayerState::Idle;

	// Handle grab interaction
	if (Game::getKey(KEY_GRAB) && (m_throwable_obj != nullptr) && m_can_grab && m_state != PlayerState::Hurt)
	{
		m_has_object = !m_has_object;
		m_can_grab = false;

		if (m_has_object)
		{
			m_throwable_obj->onPickUp();
		}
		else
		{
			m_throwable_obj->onThrow(m_looking_right, (m_vel != glm::vec2(0.f,0.f)));

			if (!m_throwable_obj->isDestroyedOnImpact())
			{
				if (m_looking_right)
					m_throwable_obj->setPosition(m_pos + glm::ivec2(16.f * 4.f, -24.f * 4));
				else
					m_throwable_obj->setPosition(m_pos + glm::ivec2(-16.f * 4.f, -24.f * 4));
			}

			m_throwable_obj = nullptr;
		}
	}

	if (!m_can_grab && !Game::getKey(KEY_GRAB))
	{
		m_can_grab = true;
	}

	//////// X AXIS
	if (Game::getKey(KEY_MOVE_LEFT))
	{
		m_sprite->turnLeft();
		m_looking_right = false;

		// Don't move if crouching
		if (!Game::getKey(KEY_MOVE_DOWN))
		{
			if (m_grounded && m_state != PlayerState::Hurt)
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
		if (!Game::getKey(KEY_MOVE_DOWN))
		{
			if (m_grounded && m_state != PlayerState::Hurt)
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
	else if (m_grounded && m_state != PlayerState::Hurt)
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
	if (Game::getKey(KEY_MOVE_DOWN) && !m_has_object && m_state != PlayerState::Hurt)
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

	//////// Y AXIS

	if (m_grounded)
	{
		if (m_jumped)
			m_jumped = false;

		// Jump
		if (Game::getKey(KEY_JUMP) && m_state != PlayerState::Hurt)
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
			m_vel.y = calculateJumpVelocity(JUMP_HEIGHT/2, S_GRAVITY);
			m_jump_counter = 0;
			m_jumped = true;
		}
		else
			m_affected_by_gravity = true;
	}
	else
	{
		if (m_jumped && m_state != PlayerState::Hurt) 
		{
			if (Game::getKey(KEY_JUMP))
			{
				m_jump_counter += delta_time;
				if (m_jump_counter < 150)
				{
					m_vel.y = std::min(m_vel.y, calculateJumpVelocity(JUMP_HEIGHT/2, S_GRAVITY));
				}
			}
			else
				m_jump_counter = 1000;
		}

		// Update from going up to going down
		if (m_vel.y > 0 && m_state != PlayerState::Hurt)
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

	// This updates the position and velocity
	Entity::update(delta_time);

	// Change animation if necessary
	if (prev_state != m_state)
	{
		switch (m_state)
		{
			case PlayerState::Idle:
				m_sprite->changeAnimation(IDLE);
				break;
			case PlayerState::Moving:
				m_sprite->changeAnimation(MOVE);
				break;
			case PlayerState::Crouching:
				m_sprite->changeAnimation(CROUCH);
				break;
			case PlayerState::Jumping:
				m_sprite->changeAnimation(JUMP);
				break;
			case PlayerState::Falling:
				m_sprite->changeAnimation(FALL);
				break;
			case PlayerState::Sliding:
				m_sprite->changeAnimation(SLIDE);
				break;
			case PlayerState::AttackingUp:
				m_sprite->changeAnimation(ATTACK_UP);
				break;
			case PlayerState::AttackingDown:
				m_sprite->changeAnimation(ATTACK_DOWN);
				break;
			case PlayerState::HoldIdle:
				m_sprite->changeAnimation(HOLD_IDLE);
				break;
			case PlayerState::HoldMoving:
				m_sprite->changeAnimation(HOLD_MOVE);
				break;
			case PlayerState::HoldJumping:
				m_sprite->changeAnimation(HOLD_JUMP);
				break;
			case PlayerState::HoldFalling:
				m_sprite->changeAnimation(HOLD_FALL);
				break;
			case PlayerState::Hurt:
				m_sprite->changeAnimation(HURT);
				break;
		}
	}

	// Update sprite position
	m_sprite->setPosition(glm::vec2(static_cast<float>(m_tilemap_displ.x + m_pos.x), static_cast<float>(m_tilemap_displ.y + m_pos.y)));

	// Update holding object position
	if (m_has_object)
	{
		if (m_looking_right)
			m_throwable_obj->setPosition(m_pos + glm::ivec2(4.f,-24.f*4));
		else
			m_throwable_obj->setPosition(m_pos + glm::ivec2(-4.f,-24.f*4));
	}
	else 
	{
		m_throwable_obj = nullptr;
	}
}

void Player::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
	{
	case EntityType::Enemy: 
	{
		if (!isAttacking())
			takeHit();
		else 
		{
			auto enemy = static_cast<Enemy*>(collision.entity);
			gainPoints(enemy->getPoints());
			m_vel.y = S_BOUNCE_SPEED;
		}
		return;
	}
	case EntityType::Projectile:
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
	case EntityType::Gem: 
	{
		changeScreen(Screen::Credits);
		return;
	}
	case EntityType::ThrowableTile: 
	{
		auto throwable = static_cast<ThrowableTile*>(collision.entity);
		if (throwable->isStatic()) 
		{
			// Player can only grab one object at a time
			if (!m_throwable_obj && (Game::getKey(KEY_MOVE_LEFT) || Game::getKey(KEY_MOVE_RIGHT)))
			{
				glm::ivec2 me_to_object = throwable->getPosition() - m_pos;

				// The Y coordinates have to practically match
				int const MAX_Y_DIFFERENCE = 8;
				if (me_to_object.y < MAX_Y_DIFFERENCE && ((m_looking_right && me_to_object.x > 0) || (!m_looking_right && me_to_object.x < 0)))
					m_throwable_obj = throwable;
			}

			// If I'm attacking, bounce on it
			if (isAttacking() && throwable->isDestroyedOnImpact())
				m_vel.y = S_BOUNCE_SPEED;
			else 
				computeCollisionAgainstSolid(throwable);
		}
		return;
	}
	case EntityType::Platform:
		computeCollisionAgainstSolid(collision.entity);
		return;
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
	if (!m_invulnerable && !m_god_mode)
	{
		--m_power;

		m_ui->setPower(m_power);

		if (m_power <= 0)
		{
			loseTry();
		}

		// The hurt animation is different from all the other ones in the sense that it only plays
		// for a specific time and it has to finish, so we set a timer to turn it off after that time
		m_hurt = true;
		auto stopHurtAnimation = [this]()
			{
				m_hurt = false;
			};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(400, stopHurtAnimation));

		// Trigger invulnerability for a while
		m_invulnerable = true;
		m_sprite->startFlickering();

		auto stopInvulnerability = [this]()
			{
				m_invulnerable = false;
				m_sprite->stopFlickering();
			};
		TimedEvents::pushEvent(std::make_unique<TimedEvent>(m_invulnerability_time, stopInvulnerability));

		// Play sound
	}
}

void Player::loseTry()
{
	auto OnDie = [this]()
	{
		if (m_throwable_obj && m_has_object)
			m_throwable_obj->onThrow(true, true);
		m_throwable_obj = nullptr;
		m_has_object = false;
		m_sprite->changeAnimation(IDLE);

		setPosition(m_original_pos);
		m_camera->setPosition(m_original_pos);
		m_camera->setOffset(3);
		m_camera->setStatic(false);
		glClearColor(0.53f, 0.77f, 1.0f, 1.0f);

		for (auto& reactivable : m_reactivate_on_respawn)
		{
			reactivable->setEnabled(true);
			reactivable->moveToOriginalPosition();
		}
	};

	if (m_invulnerable || m_god_mode)
	{
		setPosition(m_original_pos);
		OnDie();
	}
	else
	{
		--m_tries;
		m_ui->setTries(m_tries);
		if (m_tries <= 0)
		{
			changeScreen(Screen::StrartScreen);
		}
		else
		{
			m_power = 3;
			m_ui->setPower(m_power);
			OnDie();
		}
	}
}

void Player::gainPower(unsigned int gain) 
{
	m_power = std::min(static_cast<unsigned int>(m_max_power), m_power + gain);
	m_ui->setPower(m_power);
}

void Player::gainPoints(unsigned int gain) 
{
	m_points += gain;
	m_ui->setScore(m_points);
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
	loseTry();
}

void Player::setChangeScreenCallback(std::function<void(Screen)> callback) {
	m_change_scene_callback = callback;
}

void Player::changeScreen(Screen scene_id) {
	if (m_change_scene_callback) {
		m_change_scene_callback(scene_id);
	}
}

void Player::configureAnimations()
{
	m_sprite->setNumberAnimations(COUNT);

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

	// Hurt
	// Extra frames at the end to make sure the animation doesn't loop if the timer arrives a bit late
	m_sprite->setAnimationSpeed(HURT, 8);
	m_sprite->addKeyframe(HURT, glm::vec2(4.f, 3.f));
	m_sprite->addKeyframe(HURT, glm::vec2(5.f, 3.f));
	m_sprite->addKeyframe(HURT, glm::vec2(5.f, 3.f));
	m_sprite->addKeyframe(HURT, glm::vec2(5.f, 3.f));
	m_sprite->addKeyframe(HURT, glm::vec2(5.f, 3.f));
}