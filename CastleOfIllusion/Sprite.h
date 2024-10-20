#ifndef _SPRITE_INCLUDE
#define _SPRITE_INCLUDE

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AnimKeyframes.h"

// This class is derived from code seen earlier in TexturedQuad but it is also
// able to manage animations stored as a spritesheet. 
class Sprite
{

public:
	// Textured quads can only be created inside an OpenGL context
	static Sprite* createSprite(glm::ivec2 quad_size, glm::vec2 size_in_spritesheet,
		                        std::shared_ptr<Texture> spritesheet, std::shared_ptr<ShaderProgram> program);

	~Sprite() { free(); }

	// Updates the sprite
	void update(int delta_time);

	// Renders the sprite
	void render() const;

	// Sets the number of animations of the sprite
	void setNumberAnimations(int num_animations);

	// Sets the speed of the animation
	void setAnimationSpeed(int animation_id, int keyframes_per_sec);

	// Sets the offset inside the texture coordinates, assuming the position is the top left corner of the miniquad
	void setTextureCoordsOffset(glm::vec2 offset);
	
	// Adds a keyframe to the specified animation
	void addKeyframe(int animation_id, glm::vec2 frame);

	// Changes the current animation
	void changeAnimation(int animation_id);

	// Returns the current animation
	int animation() const;
	
	// Sets the position of the sprite
	void setPosition(glm::vec2 pos);

	glm::ivec2 getQuadSize() const;

	void turnRight();
	void turnLeft();

private:
	// Private constructor for the factory pattern
	Sprite(glm::ivec2 quad_size, glm::vec2 size_in_spritesheet, std::shared_ptr<Texture> spritesheet, 
		   std::shared_ptr<ShaderProgram> program);

	// Cleans up resources
	void free();

	// The texture used for the sprite
	std::shared_ptr<Texture> m_texture;

	// The shader program used to render this sprite
	std::shared_ptr<ShaderProgram> m_shader_program;

	// The VAO for this object
	GLuint m_vao;

	// The VBO for this object
	GLuint m_vbo;

	// The location of the position in the shader
	GLint m_pos_location; 

	// The location of the texture coords in the shader
	GLint m_texcoord_location;

	// The position of the sprite
	glm::vec2 m_position;

	// The x and y sizes of the quad
	glm::ivec2 m_quad_size;

	// The currently selected animation
	int m_current_animation;

	// The currently selected keyframe
	int m_current_keyframe;

	// The time of the animation
	float m_time_animation;

	// The texture coordinates offset, ie. the position of the top left corner of the miniquad inside the texture
	glm::vec2 m_texcoord_displ;

	glm::vec2 m_size_in_spritesheet;

	// The different animations the sprite may have
	std::vector<AnimKeyframes> m_animations;
};


#endif // _SPRITE_INCLUDE
