#ifndef _SHADER_INCLUDE
#define _SHADER_INCLUDE


#include <string>
#include <GL/glew.h>
#include <GL/gl.h>


enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };


// This class is able to load to OpenGL a vertex or fragment shader and compile it.
// It can do so from a file or from a std::string so that shader code can be
// procedurally modified if needed.


class Shader
{

public:
	Shader() = default;

	// Not allowing moving and copying because it doesn't make sense for this case
	Shader(Shader const& other) = delete;
	Shader(Shader&& other) = delete;
	Shader& operator=(Shader const& other) = delete;
	Shader& operator=(Shader&& other) = delete;


	~Shader() { free(); }

	// These methods should be called with an active OpenGL context

	// Initializes the shader with the source code
	// Should be called with an active OpenGL context
	void initFromSource(ShaderType const type, std::string const& source);

	// Initializes the shader from a file
	// Should be called with an active OpenGL context
	bool initFromFile(ShaderType const type, std::string const& filename);

	// Returns the OpenGL ID of this shader
	GLuint getId() const;

	// Returns true iff the shader is correctly compiled
	bool isCompiled() const;

	// Returns the error log string, if any
	std::string const& log() const;

private:
	bool loadShaderSource(const std::string &filename, std::string &shaderSource);

private:
	// Cleans up resources
	void free();

	// The OpenGL ID for this shader
	GLuint m_shader_id = 0;

	// True iff the shader has compiled (and correctly)
	bool m_compiled = false;

	// The error log if an error has happened
	std::string m_error_log;

};


#endif // _SHADER_INCLUDE
