#include "Text.h"
#include <cmath>

Text::Text(glm::vec2 const& pos, glm::ivec2 const& char_size, std::shared_ptr<ShaderProgram> shader_program)
{
	m_pos = pos;
	m_char_sprite_size = char_size;
	m_shader_program = shader_program;

	m_spritesheet.reset(new Texture());
	m_spritesheet->loadFromFile("images/Numbers.png", TEXTURE_PIXEL_FORMAT_RGBA);

	int num_digits = 6;
	m_sprites.resize(num_digits);
	for (int i = 0; i < num_digits; ++i)
		m_sprites[i].reset(Sprite::createSprite(m_char_sprite_size, glm::vec2(0.0625f, 1.0f), m_spritesheet, m_shader_program));
}

void Text::writeNumber(int number, int num_digits, glm::vec2 const& pos)
{
	m_pos = pos;
	//m_sprites.clear();
	//m_sprites.resize(num_digits); // = std::vector<std::shared_ptr<Sprite>>(num_digits);
	for (int i = 0; i < num_digits; ++i)
	{
		//m_sprites[i].reset(Sprite::createSprite(m_char_sprite_size, glm::vec2(0.0625f, 1.0f), m_spritesheet, m_shader_program));

		int digit = (number / static_cast<int>(std::pow(10, num_digits - i - 1))) % 10;

		m_sprites[i]->setTextureCoordsOffset(glm::vec2(digit * 0.0625f, 0.0f));

		m_sprites[i]->setPosition(glm::vec2((m_pos.x + m_char_sprite_size.x * i), (m_pos.y)));
	}
}

void Text::setPosition(glm::vec2 pos)
{
	for (int i = 0; i < m_sprites.size(); ++i)
	{
		m_sprites[i]->setPosition(glm::vec2(static_cast<float>(pos.x + m_char_sprite_size.x * i), static_cast<float>(pos.y)));
	}
}

void Text::render()
{
	for (int i = 0; i < m_sprites.size(); ++i)
	{
		m_sprites[i]->render();
	}
}