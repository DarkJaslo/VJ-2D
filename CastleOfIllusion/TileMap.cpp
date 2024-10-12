#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram& program)
{
	TileMap *map = new TileMap(level_file, min_coords, program);
	return map;
}


TileMap::TileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram& program)
{
	loadLevel(level_file);
	prepareArrays(min_coords, program);
}

TileMap::~TileMap()
{
	free();
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	m_tilesheet.use();
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(m_pos_location);
	glEnableVertexAttribArray(m_texcoord_location);
	glDrawArrays(GL_TRIANGLES, 0, 6 * m_num_tiles);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteBuffers(1, &m_vbo);
}

bool TileMap::loadLevel(std::string const& level_file)
{
	ifstream fin;
	string line, tilesheet_file;
	stringstream sstream;
	char tile_row;
	char tile_column;
	
	// Open level file
	fin.open(level_file.c_str());
	if(!fin.is_open())
		return false;

	// Check it is indeed a level file
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;

	// Read the map size in tiles
	getline(fin, line);
	sstream.str(line);
	sstream >> m_map_size.x >> m_map_size.y;

	// Read the tile and block size
	getline(fin, line);
	sstream.str(line);
	sstream >> m_tile_size >> m_block_size;

	// Read the path to the tilesheet file
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheet_file;

	// Load and configure the tilesheet texture
	m_tilesheet.loadFromFile(tilesheet_file, TEXTURE_PIXEL_FORMAT_RGBA);
	m_tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	m_tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	m_tilesheet.setMinFilter(GL_NEAREST);
	m_tilesheet.setMagFilter(GL_NEAREST);

	// Read the tilesheet size (in tiles)
	getline(fin, line);
	sstream.str(line);
	sstream >> m_tilesheet_size.x >> m_tilesheet_size.y;
	m_tile_tex_size = glm::vec2(1.f / m_tilesheet_size.x, 1.f / m_tilesheet_size.y);
	
	m_map.clear();
	m_map.resize(m_map_size.x * m_map_size.y);
	for (int j=0; j<m_map_size.y; j++)
	{
		for (int i=0; i<m_map_size.x; i++)
		{
			fin.get(tile_row);
			if (tile_row != '.') {
				fin.get(tile_column);
				if (tile_column == '.')
					return false;
				else
				{
					int row = tile_row - int('a');
					int column = tile_column - int('a');
					m_map[j * m_map_size.x + i] = row* m_tilesheet_size.x+column;
				}
			}
			else
			{
				fin.get(tile_row);
				m_map[j * m_map_size.x + i] = -1;
			}
		}
		fin.get(tile_row);
#ifndef _WIN32
		fin.get(tile_row);
#endif
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(glm::vec2 const& min_coords, ShaderProgram& program)
{
	int tile;
	glm::vec2 pos_tile, texcoord_tile[2], half_texel;
	vector<float> vertices;
	
	m_num_tiles = 0;
	half_texel = glm::vec2(0.5f / m_tilesheet.width(), 0.5f / m_tilesheet.height());
	for (int j=0; j<m_map_size.y; j++)
	{
		for (int i=0; i<m_map_size.x; i++)
		{
			tile = m_map[j * m_map_size.x + i];
			if (tile != -1)
			{
				// Non-empty tile
				m_num_tiles++;
				pos_tile = glm::vec2(min_coords.x + i * m_tile_size, min_coords.y + j * m_tile_size);
				texcoord_tile[0] = glm::vec2(float((tile)%m_tilesheet_size.x) / m_tilesheet_size.x,
											 float((tile)/m_tilesheet_size.x) / m_tilesheet_size.y);
				texcoord_tile[1] = texcoord_tile[0] + m_tile_tex_size;
				//texcoord_tile[0] += half_texel;
				texcoord_tile[1] -= half_texel;
				// First triangle
				vertices.push_back(pos_tile.x); vertices.push_back(pos_tile.y);
				vertices.push_back(texcoord_tile[0].x); vertices.push_back(texcoord_tile[0].y);
				vertices.push_back(pos_tile.x + m_block_size); vertices.push_back(pos_tile.y);
				vertices.push_back(texcoord_tile[1].x); vertices.push_back(texcoord_tile[0].y);
				vertices.push_back(pos_tile.x + m_block_size); vertices.push_back(pos_tile.y + m_block_size);
				vertices.push_back(texcoord_tile[1].x); vertices.push_back(texcoord_tile[1].y);
				// Second triangle
				vertices.push_back(pos_tile.x); vertices.push_back(pos_tile.y);
				vertices.push_back(texcoord_tile[0].x); vertices.push_back(texcoord_tile[0].y);
				vertices.push_back(pos_tile.x + m_block_size); vertices.push_back(pos_tile.y + m_block_size);
				vertices.push_back(texcoord_tile[1].x); vertices.push_back(texcoord_tile[1].y);
				vertices.push_back(pos_tile.x); vertices.push_back(pos_tile.y + m_block_size);
				vertices.push_back(texcoord_tile[0].x); vertices.push_back(texcoord_tile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * m_num_tiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	m_pos_location = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	m_texcoord_location = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

std::optional<glm::vec2> TileMap::xCollision(glm::ivec2 const& pos, glm::ivec2 const& size, glm::vec2 const& velocity) const
{
	int left, right, top, bottom;
	
	left = pos.x / m_tile_size;
	right = (pos.x + size.x - 1) / m_tile_size;
	top = pos.y / m_tile_size;
	bottom = (pos.y + size.y - 1) / m_tile_size;

	if (velocity.x > 0.f) // Moving right
	{
		for (int y=top; y<=bottom; y++)
		{
			if (m_map[y*m_map_size.x+right] != -1)
				return glm::vec2(m_tile_size * right - size.x, pos.y);
		}
	}
	else { // Moving left
		for (int y=top; y<=bottom; y++)
		{
			if (m_map[y*m_map_size.x+left] != -1)
				return glm::vec2(m_tile_size * (left+1), pos.y);
		}
	}

	return std::nullopt;
}

std::optional<glm::vec2> TileMap::yCollision(glm::ivec2 const& pos, glm::ivec2 const& size, glm::vec2 const& velocity) const
{
	int left, right, top, bottom;
	
	left = pos.x / m_tile_size;
	right = (pos.x + size.x - 1) / m_tile_size;
	top = pos.y / m_tile_size;
	bottom = (pos.y + size.y - 1) / m_tile_size;

	if (velocity.y > 0.f) // Falling
	{
		for (int x=left; x<=right; x++)
		{
			if (m_map[bottom*m_map_size.x+x] != -1)
			{
				return glm::vec2(pos.x, m_tile_size * bottom - size.y);
			}
		}
	}
	else
	{
		for (int x=left; x<=right; x++)
		{
			if (m_map[top*m_map_size.x+x] != -1)
			{
				return glm::vec2(pos.x, m_tile_size * (top+1));
			}
		}
	}
	return std::nullopt;
}

bool TileMap::isGrounded(glm::ivec2 const& pos, glm::ivec2 const& size) const
{
	int left, right, y;
	
	left = pos.x / m_tile_size;
	right = (pos.x + size.x - 1) / m_tile_size;
	y = (pos.y + size.y) / m_tile_size;
	for (int x=left; x<=right; x++)
	{
		if (m_map[y*m_map_size.x+x] != -1)
		{
			return true;
		}
	}
	return false;
}
