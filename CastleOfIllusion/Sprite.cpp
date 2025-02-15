#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"


Sprite *Sprite::createSprite(glm::ivec2 quad_size, glm::vec2 size_in_spritesheet, std::shared_ptr<Texture> spritesheet, 
	                         std::shared_ptr<ShaderProgram> program)
{
	Sprite* sprite = new Sprite(quad_size, size_in_spritesheet, spritesheet, program);
	return sprite;
}


Sprite::Sprite(glm::ivec2 quad_size, glm::vec2 size_in_spritesheet, std::shared_ptr<Texture> spritesheet, 
	           std::shared_ptr<ShaderProgram> program)
{
	float vertices[24] = {0.f, 0.f, 0.f, 0.f, 
						  quad_size.x, 0.f, size_in_spritesheet.x, 0.f, 
						  quad_size.x, quad_size.y, size_in_spritesheet.x, size_in_spritesheet.y, 
						  0.f, 0.f, 0.f, 0.f, 
						  quad_size.x, quad_size.y, size_in_spritesheet.x, size_in_spritesheet.y, 
						  0.f, quad_size.y, 0.f, size_in_spritesheet.y};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
	m_pos_location = program->bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	m_texcoord_location = program->bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
	m_texture = spritesheet;
	m_shader_program = program;
	m_current_keyframe = 0;
	m_current_animation = -1;
	m_time_animation = 0.f;
	m_position = glm::vec2(0.f);
	m_quad_size = quad_size;
	m_size_in_spritesheet = size_in_spritesheet;
}

void Sprite::update(int delta_time)
{
	if(m_current_animation >= 0)
	{
		m_time_animation += delta_time;
		while(m_time_animation > m_animations[m_current_animation].millisecsPerKeyframe)
		{
			m_time_animation -= m_animations[m_current_animation].millisecsPerKeyframe;
			m_current_keyframe = (m_current_keyframe + 1) % m_animations[m_current_animation].keyframeDispl.size();
		}
		m_texcoord_displ = m_animations[m_current_animation].keyframeDispl[m_current_keyframe];
	}
	if (m_flicker)
		m_flicker_counter++;
}

void Sprite::render() const
{
	if (!m_flicker || (m_flicker_counter % 4) < 2) // flicker 2 consecutive frames every 4 frames
	{
		glm::mat4 modelview = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, 0.f));
		m_shader_program->setUniformMatrix4f("modelview", modelview);
		m_shader_program->setUniform2f("texCoordDispl", m_texcoord_displ.x, m_texcoord_displ.y);
		glEnable(GL_TEXTURE_2D);
		m_texture->use();
		glBindVertexArray(m_vao);
		glEnableVertexAttribArray(m_pos_location);
		glEnableVertexAttribArray(m_texcoord_location);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_TEXTURE_2D);
	}
}

void Sprite::free()
{
	glDeleteBuffers(1, &m_vbo);
}

void Sprite::setNumberAnimations(int num_animations)
{
	m_animations.clear();
	m_animations.resize(num_animations);
}

void Sprite::setAnimationSpeed(int animation_id, int keyframes_per_sec)
{
	if(animation_id < static_cast<int>(m_animations.size()))
		m_animations[animation_id].millisecsPerKeyframe = 1000.f / keyframes_per_sec;
}

void Sprite::setTextureCoordsOffset(glm::vec2 offset) 
{
	m_texcoord_displ = offset;
}

void Sprite::addKeyframe(int animation_id, glm::vec2 displacement)
{
	if(animation_id < static_cast<int>(m_animations.size()))
		m_animations[animation_id].keyframeDispl.push_back(displacement*m_size_in_spritesheet);
}

void Sprite::changeAnimation(int animation_id)
{
	if(animation_id < static_cast<int>(m_animations.size()))
	{
		m_current_animation = animation_id;
		m_current_keyframe = 0;
		m_time_animation = 0.f;
		m_texcoord_displ = m_animations[animation_id].keyframeDispl[0];
	}
}

int Sprite::animation() const
{
	return m_current_animation;
}

void Sprite::setPosition(glm::vec2 pos)
{
	m_position = pos - glm::vec2(static_cast<float>(m_quad_size.x) / 2.0f, m_quad_size.y);
}

glm::ivec2 Sprite::getQuadSize() const
{
	return m_quad_size;
}

void Sprite::turnRight()
{
	float vertices[24] = { 0.f, 0.f, 0.f, 0.f,
						  m_quad_size.x, 0.f, m_size_in_spritesheet.x, 0.f,
						  m_quad_size.x, m_quad_size.y, m_size_in_spritesheet.x, m_size_in_spritesheet.y,
						  0.f, 0.f, 0.f, 0.f,
						  m_quad_size.x, m_quad_size.y, m_size_in_spritesheet.x, m_size_in_spritesheet.y,
						  0.f, m_quad_size.y, 0.f, m_size_in_spritesheet.y };

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
}
void Sprite::turnLeft()
{
	float vertices[24] = { 0.f, 0.f, m_size_in_spritesheet.x, 0.f,
						  m_quad_size.x, 0.f, 0.f, 0.f,
						  m_quad_size.x, m_quad_size.y, 0.f, m_size_in_spritesheet.y,
						  0.f, 0.f, m_size_in_spritesheet.x, 0.f,
						  m_quad_size.x, m_quad_size.y, 0.f, m_size_in_spritesheet.y,
						  0.f, m_quad_size.y, m_size_in_spritesheet.x, m_size_in_spritesheet.y };

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
}

void Sprite::startFlickering()
{
	m_flicker = true;
}

void Sprite::stopFlickering()
{
	m_flicker = false;
	m_flicker_counter = 0;
}
