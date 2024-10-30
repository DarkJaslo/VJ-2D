#include "CameraPoint.h"
#include "TimedEvent.h"


CameraPoint::CameraPoint(glm::ivec2 upleft_corner_pos, 
	glm::ivec2 collision_size, 
	std::shared_ptr<Camera> camera, 
	std::shared_ptr<Player> player, 
	glm::ivec2 player_spawn_point, 
	int camera_offset, 
	std::shared_ptr<ShaderProgram> shader_program,
	int identifier,
	std::shared_ptr<Boss> boss)
{
	if (identifier >= Identifier::None)
		throw std::runtime_error("Identifier is too big!");

	if (identifier == 0)
		m_id = Identifier::GreenToSky;
	else if (identifier == 1)
		m_id = Identifier::SkyToSand;
	else
		m_id = Identifier::SandToBoss;

	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
	m_bounces = false;
	m_camera = camera;
	m_player_spawn_point = player_spawn_point;
	m_player = player;
	m_camera_offset = camera_offset;
	m_boss = boss;

	if (collision_size.x % 2 != 0)
		throw std::runtime_error("CameraPoint::CameraPoint: Please provide a multiple of 2 for the X collision size");

	m_pos = glm::ivec2(upleft_corner_pos.x + collision_size.x / 2, upleft_corner_pos.y + collision_size.y);
	m_original_pos = m_pos;
	m_collision_box_size = collision_size;
}

void CameraPoint::collideWithEntity(Collision collision)
{
    if (!m_enabled)
        return;

    switch (collision.entity->getType())
    {
    case EntityType::Player:
    {
		if (m_id == Identifier::SandToBoss) 
		{
			if (m_player->m_throwable_obj)
				m_player->m_throwable_obj->setEnabled(false);

			m_player->m_throwable_obj = nullptr;
			m_player->m_has_object = false;
			// IDLE
			m_player->m_sprite->changeAnimation(0);

			auto EnableBoss = [this]()
			{
				m_boss->setEnabled(true);
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(1400, EnableBoss));

			m_camera->m_pos.x = (83 * 64 - 32) - m_camera->m_size.x / 2;
			m_camera->setStatic(true);
		}
		else if (m_id == Identifier::SkyToSand) 
		{
			auto SetColor = []()
			{
				glClearColor(64.0f / 255.0f, 33.0f / 255.0f, 16.0f / 255.0f, 1.0f);
			};
			TimedEvents::pushEvent(std::make_unique<TimedEvent>(800, SetColor));
		}

		m_camera->scrollToPoint(m_camera->getPosition() + glm::vec2(0.f, m_camera_offset * 16.f * 4.f), 1000.f);
		m_player->setSpawnPosition(m_player_spawn_point);
		m_can_collide = false;
		m_enabled = false;
        break;
    }
	case EntityType::ThrowableTile: 
	{
		collision.entity->setEnabled(false);
		break;
	}
	case EntityType::Platform: 
	{
		collision.entity->setEnabled(false);
		break;
	}
    default:
        break;
    }
}

void CameraPoint::setEnabled(bool enabled) 
{
	Entity::setEnabled(enabled);

	m_can_collide = true;
	m_enabled = true;
}