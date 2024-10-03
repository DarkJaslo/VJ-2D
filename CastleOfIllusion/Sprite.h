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
	static Sprite* createSprite(glm::vec2 const& quad_size, glm::vec2 const& size_in_spritesheet,
		std::shared_ptr<Texture> spritesheet, std::shared_ptr<ShaderProgram> program);

	~Sprite() { free(); }

	// Updates the sprite
	void update(int delta_time);

	// Renders the sprite
	void render() const;

	// Sets the number of frames of the animation
	void setNumberAnimations(int num_animations);

	// Sets the speed of the animation
	void setAnimationSpeed(int animation_id, int keyframes_per_sec);
	
	// Adds a keyframe to the specified animation
	void addKeyframe(int animation_id, glm::vec2 const& frame);

	// Changes the current animation
	void changeAnimation(int animation_id);

	// Returns the current animation
	int animation() const;
	
	// Sets the position of the sprite
	void setPosition(glm::vec2 const& pos);

private:
	// Private constructor for the factory pattern
	Sprite(glm::vec2 const& quad_size, glm::vec2 const& size_in_spritesheet, std::shared_ptr<Texture> spritesheet, 
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

	// The currently selected animation
	int m_current_animation;

	// The currently selected keyframe
	int m_current_keyframe;

	// The time of the animation
	float m_time_animation;

	// ?
	glm::vec2 m_texcoord_displ;

	// The different animations the sprite may have
	vector<AnimKeyframes> m_animations;
};


#endif // _SPRITE_INCLUDE
