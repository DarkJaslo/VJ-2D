#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include <optional>


// Class Tilemap is capable of loading a tile map from a text file in a simple format
// (see test.txt for an example). Tiles are indexed with 2 characters from a to z refering
// to its row and column in the tilesheet, a being 0 and z being 26. An empty tile is 
// represented with '..'. With this information it builds a single VBO that contains all tiles.
// As a result the render method draws the whole map independently of what is visible.




class TileMap
{

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram &program);

	~TileMap();

	// Renders the tilemap
	void render() const;
	
	// Returns the size of one tile
	int getTileSize() const { return m_tile_size; }

	// Checks if a rectangle at the position and of the size provided collides horizontally with a tile in the 
	// tilemap, if so, returns the position it would be right before colliding
	std::optional<glm::ivec2> xCollision(glm::ivec2 const& pos, glm::ivec2 const& size, glm::vec2 const& velocity) const;
	
	// Checks if a rectangle at the position and of the size provided collides vertically with a tile in the 
	// tilemap, if so, returns the position it would be right before colliding
	std::optional<glm::ivec2> yCollision(glm::ivec2 const& pos, glm::ivec2 const& size, glm::vec2 const& velocity) const;
	
	bool isGrounded(glm::ivec2 const& pos, glm::ivec2 const& size) const;
	
private:
	// Private constructor for the factory pattern
	TileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram& program);

	void free();

	// Loads a level
	bool loadLevel(std::string const& level_file);
	void prepareArrays(glm::vec2 const& min_coords, ShaderProgram& program);

private:
	// The tilemap's VAO
	GLuint m_vao;

	// The tilemap's VBO
	GLuint m_vbo;

	// The pos location in the shader
	GLint m_pos_location; 

	// The texture coord location in the shader
	GLint m_texcoord_location;

	// The number of tiles
	int m_num_tiles;

	glm::ivec2 m_position;

	glm::ivec2 m_map_size;
	
	// The size of the tilesheet
	glm::ivec2 m_tilesheet_size;
	
	// The size of each tile
	int m_tile_size;

	// The size of each block
	int m_block_size;

	// The tilesheet
	Texture m_tilesheet;

	// The size of the texture of the tiles
	glm::vec2 m_tile_tex_size;

	// The map
	// Stores the index of each tile, -1 indicates an empty tile
	std::vector<int> m_map;

};


#endif // _TILE_MAP_INCLUDE


