#include "Void.h"

Void::Void(glm::ivec2 upleft_corner_pos, glm::ivec2 collision_size, std::shared_ptr<ShaderProgram> shader_program)
{
	m_affected_by_gravity = false;
	m_affected_by_x_drag = false;
	m_bounces = false;

	if (collision_size.x % 2 != 0)
		throw std::runtime_error("Void::Void: Please provide a multiple of 2 for the X collision size");

	m_pos = glm::ivec2(upleft_corner_pos.x + collision_size.x / 2, upleft_corner_pos.y + collision_size.y);
	m_collision_box_size = collision_size;

	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile("images/Blocks2.png", TEXTURE_PIXEL_FORMAT_RGBA);
	glm::vec2 size_in_texture = glm::vec2(0.125f, 0.5f);
	m_sprite.reset(Sprite::createSprite(glm::ivec2(collision_size.x, collision_size.y) /* quad_size */,
		size_in_texture,
		m_spritesheet,
		shader_program));
	m_sprite->setPosition(m_pos);
	m_sprite->setTextureCoordsOffset(glm::vec2(0.125f*6, 0.5f));
}