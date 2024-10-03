#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram &program)
{
	TileMap *m_map = new TileMap(level_file, min_coords, program);
	return m_map;
}


TileMap::TileMap(std::string const& level_file, glm::vec2 const& min_coords, ShaderProgram &program)
{
	loadLevel(level_file);
	prepareArrays(min_coords, program);
}

TileMap::~TileMap()
{
	if(m_map != NULL)
		delete m_map;

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

bool TileMap::loadLevel(const string &level_file)
{
	ifstream fin;
	string line, tilesheet_file;
	stringstream sstream;
	char tile;
	
	fin.open(level_file.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> m_map_size.x >> m_map_size.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> m_tile_size >> m_block_size;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheet_file;
	m_tilesheet.loadFromFile(tilesheet_file, TEXTURE_PIXEL_FORMAT_RGBA);
	m_tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	m_tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	m_tilesheet.setMinFilter(GL_NEAREST);
	m_tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> m_tilesheet_size.x >> m_tilesheet_size.y;
	m_tile_tex_size = glm::vec2(1.f / m_tilesheet_size.x, 1.f / m_tilesheet_size.y);
	
	m_map = new int[m_map_size.x * m_map_size.y];
	for(int j=0; j<m_map_size.y; j++)
	{
		for(int i=0; i<m_map_size.x; i++)
		{
			fin.get(tile);
			if(tile == ' ')
				m_map[j*m_map_size.x+i] = 0;
			else
				m_map[j*m_map_size.x+i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(const glm::vec2 &min_coords, ShaderProgram &program)
{
	int tile;
	glm::vec2 pos_tile, texcoord_tile[2], half_texel;
	vector<float> vertices;
	
	m_num_tiles = 0;
	half_texel = glm::vec2(0.5f / m_tilesheet.width(), 0.5f / m_tilesheet.height());
	for(int j=0; j<m_map_size.y; j++)
	{
		for(int i=0; i<m_map_size.x; i++)
		{
			tile = m_map[j * m_map_size.x + i];
			if(tile != 0)
			{
				// Non-empty tile
				m_num_tiles++;
				pos_tile = glm::vec2(min_coords.x + i * m_tile_size, min_coords.y + j * m_tile_size);
				texcoord_tile[0] = glm::vec2(float((tile-1)%m_tilesheet_size.x) / m_tilesheet_size.x, float((tile-1)/m_tilesheet_size.x) / m_tilesheet_size.y);
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

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size) const
{
	int x, y0, y1;
	
	x = pos.x / m_tile_size;
	y0 = pos.y / m_tile_size;
	y1 = (pos.y + size.y - 1) / m_tile_size;
	for(int y=y0; y<=y1; y++)
	{
		if(m_map[y*m_map_size.x+x] != 0)
			return true;
	}
	
	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size) const
{
	int x, y0, y1;
	
	x = (pos.x + size.x - 1) / m_tile_size;
	y0 = pos.y / m_tile_size;
	y1 = (pos.y + size.y - 1) / m_tile_size;
	for(int y=y0; y<=y1; y++)
	{
		if(m_map[y*m_map_size.x+x] != 0)
			return true;
	}
	
	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const
{
	int x0, x1, y;
	
	x0 = pos.x / m_tile_size;
	x1 = (pos.x + size.x - 1) / m_tile_size;
	y = (pos.y + size.y - 1) / m_tile_size;
	for(int x=x0; x<=x1; x++)
	{
		if(m_map[y*m_map_size.x+x] != 0)
		{
			if(*posY - m_tile_size * y + size.y <= 4)
			{
				*posY = m_tile_size * y - size.y;
				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const
{
	int x0, x1, y;

	x0 = pos.x / m_tile_size;
	x1 = (pos.x + size.x - 1) / m_tile_size;
	//y = (pos.y + size.y - 1) / m_tile_size;
	y = pos.y / m_tile_size;
	for (int x = x0; x <= x1; x++)
	{
		if (m_map[y * m_map_size.x + x] != 0)
		{
			if (*posY - m_tile_size * y - size.y <= 4)
			{
				*posY = m_tile_size * y + size.y;
				return true;
			}
		}
	}

	return false;
}





























