#include <fstream>
#include "Shader.h"

using namespace std;

void Shader::initFromSource(const ShaderType type, const string &source)
{
	const char* source_ptr = source.c_str();
	GLint status;
	char buffer[512];

	switch(type)
	{
	case VERTEX_SHADER:
		m_shader_id = glCreateShader(GL_VERTEX_SHADER);
		break;
	case FRAGMENT_SHADER:
		m_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	}

	if(m_shader_id == 0)
		return;

	glShaderSource(m_shader_id, 1, &source_ptr, NULL);
	glCompileShader(m_shader_id);
	glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &status);

	m_compiled = (status == GL_TRUE);
	glGetShaderInfoLog(m_shader_id, 512, NULL, buffer);
	m_error_log.assign(buffer);
}

bool Shader::initFromFile(const ShaderType type, const string &filename)
{
	string shader_source;

	if(!loadShaderSource(filename, shader_source))
		return false;
	initFromSource(type, shader_source);

	return true;
}

void Shader::free()
{
	glDeleteShader(m_shader_id);
	m_shader_id = 0;
	m_compiled = false;
}

GLuint Shader::getId() const
{
	return m_shader_id;
}

bool Shader::isCompiled() const
{
	return m_compiled;
}

const string &Shader::log() const
{
	return m_error_log;
}

bool Shader::loadShaderSource(const string &filename, string &shader_source)
{
	ifstream fin;
	fin.open(filename.c_str());
	if(!fin.is_open())
		return false;

	shader_source.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
	return true;
}

