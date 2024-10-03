#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"


void ShaderProgram::init()
{
	m_program_id = glCreateProgram();
}

void ShaderProgram::addShader(Shader const& shader) const
{
	glAttachShader(m_program_id, shader.getId());
}

void ShaderProgram::bindFragmentOutput(std::string const& output_name) const
{
	glBindAttribLocation(m_program_id, 0, output_name.c_str());
}

GLint ShaderProgram::bindVertexAttribute(std::string const& attrib_name, GLint size, GLsizei stride, GLvoid *first_pointer) const
{
	GLint attrib_pos;

	attrib_pos = glGetAttribLocation(m_program_id, attrib_name.c_str());
	glVertexAttribPointer(attrib_pos, size, GL_FLOAT, GL_FALSE, stride, first_pointer);

	return attrib_pos;
}

void ShaderProgram::link()
{
	GLint status;
	char buffer[512];

	glLinkProgram(m_program_id);
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &status);
	m_linked = (status == GL_TRUE);
	glGetProgramInfoLog(m_program_id, 512, NULL, buffer);
	m_error_log.assign(buffer);
}

void ShaderProgram::free()
{
	glDeleteProgram(m_program_id);
}

void ShaderProgram::use()
{
	glUseProgram(m_program_id);
}

bool ShaderProgram::isLinked() const
{
	return m_linked;
}

const std::string &ShaderProgram::log() const
{
	return m_error_log;
}

void ShaderProgram::setUniform2f(std::string const& uniform_name, float v0, float v1)
{
	GLint location = glGetUniformLocation(m_program_id, uniform_name.c_str());

	if(location != -1)
		glUniform2f(location, v0, v1);
}

void ShaderProgram::setUniform3f(std::string const& uniform_name, float v0, float v1, float v2)
{
	GLint location = glGetUniformLocation(m_program_id, uniform_name.c_str());

	if(location != -1)
		glUniform3f(location, v0, v1, v2);
}

void ShaderProgram::setUniform4f(std::string const& uniform_name, float v0, float v1, float v2, float v3)
{
	GLint location = glGetUniformLocation(m_program_id, uniform_name.c_str());

	if(location != -1)
		glUniform4f(location, v0, v1, v2, v3);
}

void ShaderProgram::setUniformMatrix4f(std::string const& uniform_name, glm::mat4 &mat)
{
	GLint location = glGetUniformLocation(m_program_id, uniform_name.c_str());

	if(location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

