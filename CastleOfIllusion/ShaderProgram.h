#ifndef _SHADER_PROGRAM_INCLUDE
#define _SHADER_PROGRAM_INCLUDE


#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Shader.h"


// Using the Shader class ShaderProgram can link a vertex and a fragment shader
// together, bind input attributes to their corresponding vertex shader names, 
// and bind the fragment output to a name from the fragment shader


class ShaderProgram
{

public:
	ShaderProgram() = default;

	~ShaderProgram() { free(); }

	// Initializes the shader program
	void init();

	// Adds a shader to the program
	void addShader(Shader const& shader) const;

	void bindFragmentOutput(std::string const& output_name) const;
	GLint bindVertexAttribute(std::string const& attrib_name, GLint size, GLsizei stride, GLvoid* first_pointer) const;
	void link();

	void use();

	// Pass uniforms to the associated shaders
	void setUniform2f(std::string const& uniform_name, float v0, float v1);
	void setUniform3f(std::string const& uniform_name, float v0, float v1, float v2);
	void setUniform4f(std::string const& uniform_name, float v0, float v1, float v2, float v3);
	void setUniformMatrix4f(std::string const& uniform_name, glm::mat4& mat);

	bool isLinked() const;
	std::string const& log() const;

private:
	// Cleans up resources
	void free();

	// The ID of this shader program
	GLuint m_program_id = 0;

	// True iff the program is linked (and successfully)
	bool m_linked = false;

	// The error log string, if any
	std::string m_error_log;
};


#endif // _SHADER_PROGRAM_INCLUDE
