#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(float width, float height, std::shared_ptr<Player> player)
{
    m_size.x = width;
    m_size.y = height;
    m_player = player;
	m_update_speed = glm::vec2(1.4f,1.4f);
}

glm::vec2 Camera::getSize() const
{
	return m_size;
}

glm::vec2 Camera::getPosition() const
{
	return m_pos;
}


glm::mat4 Camera::getProjectionMatrix() const
{
	return glm::ortho(m_pos.x, m_pos.x + m_size.x, m_pos.y + m_size.y, m_pos.y);
}

void Camera::update(int delta_time)
{
	glm::ivec2 pos_player = m_player->getPosition();
	glm::ivec2 size_player = m_player->getSpriteSize();
	glm::vec2 vel_player = m_player->getVelocity();

	// X Axis
	if (vel_player.x > 0) // Going right
	{
		// If the player has more than 1/3 of the camera to its left,
		// move the camera faster than the player so that it can catch up
		if ((pos_player.x - m_pos.x) > (m_size.x/3.f))
		{
			m_vel.x = m_update_speed.x*vel_player.x;
		}
		else
		{
			m_vel.x = 0;
		}
	}
	else // Going left
	{
		// If the player has more than 1/3 of the camera to its right
		// move the camera faster than the player so that it can catch up
		if ((pos_player.x + size_player.x - m_pos.x) < (2*m_size.x/3.f))
		{
			m_vel.x = m_update_speed.x*vel_player.x;
		}
		else
		{
			m_vel.x = 0;
		}
	}
	/*
	// Y Axis
	if (vel_player.y > 0) // Going down
	{
		// If the player has more than 1/3 of the camera above them
		// move the camera faster than the player so that it can catch up
		if ((pos_player.y - m_pos.y) > (m_size.y/3.f))
		{
			m_vel.y = m_update_speed.y*vel_player.y;
		}
		else
		{
			m_vel.y = 0;
		}
	}
	else // Going up
	{
		// If the player has more than 1/3 of the camera below them
		// move the camera faster than the player so that it can catch up
		if ((pos_player.y + size_player.y - m_pos.y) < (2*m_size.y/3.f))
		{
			m_vel.y = m_update_speed.y*vel_player.y;
		}
		else
		{
			m_vel.y = 0;
		}
	}*/
	m_pos += m_vel * static_cast<float>(delta_time);
	m_pos.x = std::max(m_pos.x, pos_player.x + size_player.x - 2.f*m_size.x/3.f); // Ensures the camera does not move too far to the left
	m_pos.x = std::min(m_pos.x, pos_player.x - m_size.x/3.f); // Ensures the camera does not move too far to the right
	//m_pos.y = std::max(m_pos.y, pos_player.y + size_player.y - 2*m_size.y/3.f); // Ensures the camera does not move too far to the top
	//m_pos.y = std::min(m_pos.y, pos_player.y - m_size.y/3.f); // Ensures the camera does not move too far to the bottom
}