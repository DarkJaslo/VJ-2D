#include "UI.h"
#include "Game.h"

#include <iostream>

enum PowerStarAnims
{
	FULL, EMPTY
};

UI::UI(std::shared_ptr<ShaderProgram> shader_program)
{
	// Get the time at creation
	m_start_application_time = glfwGetTime();

	// Base sprite
	m_base_spritesheet.reset(new Texture());
	m_base_spritesheet->loadFromFile("images/UIBase.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_base_sprite.reset(Sprite::createSprite(glm::ivec2(16*16*4,2*16*4), glm::vec2(1.f,1.f), m_base_spritesheet, shader_program));

	// Power sprites
	m_power_spritesheet.reset(new Texture());
	m_power_spritesheet->loadFromFile("images/Items.png", TEXTURE_PIXEL_FORMAT_RGBA);
	m_power_sprite = std::vector<std::shared_ptr<Sprite>>(3);
	for (int i = 0; i < m_power_sprite.size(); ++i)
	{
		m_power_sprite[i].reset(Sprite::createSprite(glm::ivec2(16 * 4, 16 * 4), glm::vec2(0.125f, 0.5f), m_power_spritesheet, shader_program));

		m_power_sprite[i]->setNumberAnimations(2);
		// Full star
		m_power_sprite[i]->setAnimationSpeed(FULL, 1);
		m_power_sprite[i]->addKeyframe(FULL, glm::vec2(5.f, 0.f));
		// Empty star
		m_power_sprite[i]->setAnimationSpeed(EMPTY, 1);
		m_power_sprite[i]->addKeyframe(EMPTY, glm::vec2(6.f, 0.f));

		m_power_sprite[i]->changeAnimation(FULL);
	}

	// Text
	m_tries_text.reset(new Text(glm::vec2(0.f,0.f), glm::ivec2(8.0f * 4.0f, 8.0f * 4.0f), shader_program));
	m_score_text.reset(new Text(glm::vec2(0.f, 0.f), glm::ivec2(8.0f * 4.0f, 8.0f * 4.0f), shader_program));
	m_time_text.reset(new Text(glm::vec2(0.f, 0.f), glm::ivec2(8.0f * 4.0f, 8.0f * 4.0f), shader_program));
}

void UI::render()
{
	m_base_sprite->render();
	m_tries_text->render();
	m_score_text->render();
	m_time_text->render();

	for (int i = 0; i < m_power_sprite.size(); ++i)
	{
		m_power_sprite[i]->render();
	}
}

void UI::update(int delta_time)
{
	// Calculate the time left
	// It doesn't work well with delta_time, it somehow builds up an error
	m_time_left = m_start_level_time - static_cast<int>((glfwGetTime()) - m_start_application_time);

	m_base_sprite->setPosition(m_pos);
	m_tries_text->writeNumber(m_tries, 2, m_pos + glm::vec2(-8.f*4.f,-7.f*4.f));
	m_score_text->writeNumber(m_score, 6, m_pos + glm::vec2(28.f*4.f,-7.f*4.f));
	m_time_text->writeNumber(m_time_left, 3, m_pos + glm::vec2(94.f*4.f,-7.f*4.f));

	// Debug
	/*if (Game::getKey(GLFW_KEY_0) && m_power > 0)
	{
		m_power--;
	}*/

	// Update power stars
	for (int i = 0; i < m_power; ++i)
	{
		if (m_power_sprite[i]->animation() != FULL)
			m_power_sprite[i]->changeAnimation(FULL);
	}
	for (int i = m_power; i < 3; ++i)
	{
		if (m_power_sprite[i]->animation() != EMPTY)
			m_power_sprite[i]->changeAnimation(EMPTY);
	}
	for (int i = 0; i < m_power_sprite.size(); ++i)
	{
		m_power_sprite[i]->setPosition(m_pos + glm::vec2(-112.f * 4.f, -7.f * 4.f) + glm::vec2(i * 16.f * 4.f, 0.f));
	}
}

void UI::setPosition(glm::vec2 pos)
{
	m_pos = pos;
}

void UI::setPower(int power)
{
	m_power = power;
}

void UI::setTries(int tries)
{
	m_tries = tries;
}

void UI::setScore(int score)
{
	m_score = score;
}
void UI::setTime(int time)
{
	m_time_left = time;
}