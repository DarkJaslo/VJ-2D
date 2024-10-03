#ifndef _TEXTURE_INCLUDE
#define _TEXTURE_INCLUDE


#include <string>
#include <GL/glew.h>


enum PixelFormat {TEXTURE_PIXEL_FORMAT_RGB, TEXTURE_PIXEL_FORMAT_RGBA};


// The texture class loads images an passes them to OpenGL
// storing the returned id so that it may be applied to any drawn primitives


class Texture
{

public:
	Texture();

	bool loadFromFile(std::string const& filename, PixelFormat format);
	void loadFromGlyphBuffer(unsigned char *buffer, int width, int height);

	void createEmptyTexture(int width, int height);
	void loadSubtextureFromGlyphBuffer(unsigned char *buffer, int x, int y, int width, int height);
	void generateMipmap();
	
	void setWrapS(GLint value);
	void setWrapT(GLint value);
	void setMinFilter(GLint value);
	void setMagFilter(GLint value);
	
	void use() const;
	
	int width() const { return m_width; }
	int height() const { return m_height; }

private:

	// The texture's width, in pixels
	// Has to be signed for library reasons
	int m_width;

	// The texture's height, in pixels
	// Has to be signed for library reasons
	int m_height;
	
	// The texture's OpenGL ID
	GLuint m_id;

	// The wrap type
	GLint m_wrap_s;
	GLint m_wrap_t;

	// The minification filter
	GLint m_minification_filter;

	// The magnification filter
	GLint m_magnification_filter;

};


#endif // _TEXTURE_INCLUDE

