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
	// Assumes the sprite is looking to the right
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
	void addKeyframe(int animation_id, glm::vec2 displacement);

	// Changes the current animation
	void changeAnimation(int animation_id);

	// Returns the current animation
	int animation() const;
	
	// Sets the position of the sprite
	void setPosition(glm::vec2 pos);

	glm::ivec2 getQuadSize() const;

	// Returns the shader program
	std::shared_ptr<ShaderProgram> getShaderProgram() const { return m_shader_program; }

	// Flips the sprite so that it looks to the right (default)
	void turnRight();

	// Flips the sprite so that it looks to the left
	void turnLeft();

	// Makes the sprite flicker until it is told to stop
	void startFlickering();

	// Makes the sprite stop flickering
	void stopFlickering();

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

	// The texture coordinates offset, ie. the position of the top left corner of the miniquad
	// inside the texture
	glm::vec2 m_texcoord_displ;

	// The size of a single sprite in the spritesheet
	glm::vec2 m_size_in_spritesheet;

	// The different animations the sprite may have
	std::vector<AnimKeyframes> m_animations;

	// True iff the sprite should currently be flickering
	bool m_flicker = false;

	// Used to count frames so that we can determine when the sprite should be visible and when not
	// when flickering
	int m_flicker_counter = 0;
};


#endif // _SPRITE_INCLUDE
