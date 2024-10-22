#include "Void.h"

Void::Void(glm::ivec2 upleft_corner_pos, glm::ivec2 collision_size)
{
	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
	m_bounces = false;

	if (collision_size.x % 2 != 0)
		throw std::runtime_error("Void::Void: Please provide a multiple of 2 for the X collision size");

	m_pos = glm::ivec2(upleft_corner_pos.x + collision_size.x / 2, upleft_corner_pos.y + collision_size.y);
	m_collision_box_size = collision_size;
}