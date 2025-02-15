#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


void Camera::init(float width, float height, std::shared_ptr<UI> ui)
{
    m_size.x = width;
    m_size.y = height;
	m_ui = ui;
	m_update_speed = glm::vec2(1.4f,1.4f);
}

void Camera::setPosition(glm::vec2 pos)
{
	m_pos.x = pos.x;
	m_pos.y = pos.y - m_size.y + 128;
}

void Camera::setOffset(int offset)
{
	m_pos.y += 64*offset;
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
	if (m_scrolling_to_point)
	{
		if ((m_scroll_to_point_speed > 0 && m_pos.y >= m_target_pos_y) || (m_scroll_to_point_speed < 0 && m_pos.y <= m_target_pos_y)) 
		{
			m_scrolling_to_point = false;
			m_pos.y = m_target_pos_y;
			m_scroll_to_point_speed = 0;
		}
		else
		{
			m_pos.y += m_scroll_to_point_speed*delta_time;
		}
	}
	if (m_can_move)
	{
		followPlayer(delta_time);
	}
	m_ui->setPosition(m_pos + glm::vec2(m_size.x / 2.f, m_size.y));
}

bool Camera::isVisible(glm::ivec2 pos, glm::ivec2 size) const 
{
	glm::ivec2 camera_min = m_pos;
	glm::ivec2 camera_max = m_pos + m_size;

	glm::ivec2 min = { pos.x - size.x / 2, pos.y - size.y };
	glm::ivec2 max = { pos.x + size.x / 2, pos.y };

	return max.x >= camera_min.x && min.x <= camera_max.x && max.y >= camera_min.y && min.y <= camera_max.y;
}

void Camera::setStatic(bool can_move)
{
	m_can_move = !can_move;
}

void Camera::scrollToPoint(glm::vec2 point, float duration)
{
	m_scrolling_to_point = true;
	m_target_pos_y = point.y;
	m_scroll_to_point_speed = (m_target_pos_y - m_pos.y) / duration;
}

void Camera::setPlayer(std::shared_ptr<Player> player)
{
	m_player = player;
}

void Camera::followPlayer(int delta_time)
{
	glm::ivec2 pos_player = m_player->getPosition();
	glm::ivec2 size_player = m_player->getSpriteSize();
	glm::vec2 vel_player = m_player->getVelocity();

	// X Axis
	if (vel_player.x > 0) // Going right
	{
		// If the player has more than 1/3 of the camera to its left,
		// move the camera faster than the player so that it can catch up
		if ((pos_player.x - m_pos.x) > (m_size.x / 3.f))
		{
			m_vel.x = m_update_speed.x * vel_player.x;
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
		if ((pos_player.x + size_player.x - m_pos.x) < (2 * m_size.x / 3.f))
		{
			m_vel.x = m_update_speed.x * vel_player.x;
		}
		else
		{
			m_vel.x = 0;
		}
	}

	m_pos += m_vel * static_cast<float>(delta_time);
	m_pos.x = std::max(m_pos.x, pos_player.x + size_player.x - 2.f * m_size.x / 3.f); // Ensures the camera does not move too far to the left
	m_pos.x = std::min(m_pos.x, pos_player.x - m_size.x / 3.f); // Ensures the camera does not move too far to the right
}