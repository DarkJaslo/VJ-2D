#ifndef _TEXT_INCLUDE
#define _TEXT_INCLUDE

#include "Sprite.h"
#include "Texture.h"
#include "ShaderProgram.h"

class Text
{
public:
	Text(glm::vec2 const& pos, glm::ivec2 const& char_size, std::shared_ptr<ShaderProgram> shader_program);
	
    // Changes the text to the number "number", written with "num_digits" digits at position "pos"
    void writeNumber(int number, int num_digits, glm::vec2 const& pos);

    // Sets the position of the start of the text
    void setPosition(glm::vec2 pos);

    // Renders the text
    void render();

private:

    // The shader program used to render this text
    std::shared_ptr<ShaderProgram> m_shader_program;

    // The spritesheet with the font
    std::shared_ptr<Texture> m_spritesheet;

    // A pointer to the sprite
    std::vector<std::shared_ptr<Sprite>> m_sprites;

    // The coordinates of the start of the text
    glm::vec2 m_pos;

    // The size of the sprite of a single character
    glm::ivec2 	m_char_sprite_size;
};

#endif // TEXT_INCLUDE