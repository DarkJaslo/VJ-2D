#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <algorithm>

#include "Scene.h"
#include "Game.h"
#include "Coin.h"
#include "Cake.h"
#include "Chest.h"
#include "TimedEvent.h"
#include "Void.h"
#include "Platform.h"
#include "Barrel.h"

// Tilemap top left screen position
#define SCREEN_X 0
#define SCREEN_Y 0

// coordinates of the tile where the player appears 
#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 8

Scene::Scene()
{
	m_tilemap.reset();
	m_player.reset();
	m_tex_program.reset(new ShaderProgram());
	m_camera.reset();
	m_ui.reset();
}

void Scene::init()
{
	initShaders();
	m_tilemap.reset(TileMap::createTileMap("levels/level1.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));

	m_ui.reset(new UI(m_tex_program));

	m_player.reset(new Player(glm::vec2(INIT_PLAYER_X_TILES * m_tilemap->getTileSize(), INIT_PLAYER_Y_TILES * m_tilemap->getTileSize()), 
		                      m_tilemap, m_ui, glm::ivec2(SCREEN_X, SCREEN_Y), m_player_sprite_size, m_player_collision_size, m_tex_program));

	m_camera.reset(new Camera(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_player, m_ui));

	m_entities.push_back(m_player);

	readSceneFile("levels/testTiles.entities");

	m_current_time = 0.0f;
}

void Scene::update(int delta_time)
{
	m_current_time += delta_time;

	// Updates scheduled events, if any
	TimedEvents::updateEvents(delta_time);

	// This includes the player, which is first of all
	for (auto& entity : m_entities) 
	{
		if (entity->isEnabled())
			entity->update(delta_time);
	}

	// Check collisions between entities (each pair once)
	for (std::size_t i = 0; i < m_entities.size(); ++i)
	{
		if (!m_entities[i]->canCollide())
			continue;

		for (std::size_t j = i+1; j < m_entities.size(); ++j)
		{
			if (!m_entities[j]->canCollide())
				continue;

			if (*m_entities[i] & *m_entities[j])
			{
				std::cout << "Detected collision between entities" << std::endl;

				auto&& [i_collision, j_collision] = *m_entities[i] | *m_entities[j];
				m_entities[i]->collideWithEntity(i_collision);
				m_entities[j]->collideWithEntity(j_collision);
			}
		}
	}

	m_camera->update(delta_time);
	m_ui->update(delta_time);
}

void Scene::render()
{
	glm::mat4 modelview;

	m_tex_program->use();
	m_tex_program->setUniformMatrix4f("projection", m_camera->getProjectionMatrix());
	m_tex_program->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	m_tex_program->setUniformMatrix4f("modelview", modelview);
	m_tex_program->setUniform2f("texCoordDispl", 0.f, 0.f);
	m_tilemap->render();

	// This includes the player, which last one rendered
	for (int i = m_entities.size() - 1; i >= 0; --i)
	{
		if (m_entities[i]->isEnabled())
			m_entities[i]->render();
	}
	m_ui->render();
}

void Scene::initShaders()
{
	Shader vertex_shader, fragment_shader;

	vertex_shader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vertex_shader.isCompiled())
	{
		std::cerr << "" << vertex_shader.log() << std::endl << std::endl;
		throw std::runtime_error("Vertex Shader compilation error!");
	}

	fragment_shader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fragment_shader.isCompiled())
	{
		std::cerr << "" << fragment_shader.log() << std::endl << std::endl;
		throw std::runtime_error("Fragment Shader compilation error!");
	}

	m_tex_program->init();
	m_tex_program->addShader(vertex_shader);
	m_tex_program->addShader(fragment_shader);
	m_tex_program->link();
	if(!m_tex_program->isLinked())
	{
		std::cerr << "" << m_tex_program->log() << std::endl << std::endl;
		throw std::runtime_error("Shader linking error!");
	}

	m_tex_program->bindFragmentOutput("outColor");
}

void Scene::readSceneFile(std::string&& path)
{
	std::fstream file(path);

	if (!file.is_open())
		throw std::runtime_error("Could not read scene file!");

	std::string line;

	while (getline(file, line))
	{
		std::istringstream split_line(line);
		std::string word;

		// Get entity type
		split_line >> word;

		if (word == "chest")
			createChest(split_line);
		else if (word == "void")
			createVoid(split_line);
		else if (word == "platform")
			createPlatform(split_line);
		else if (word == "barrel")
			createBarrel(split_line);
		else 
		{
			std::cerr << "Scene::readSceneFile: wrong word: " << word << std::endl;
			throw std::runtime_error("");
		}
	}
}

void Scene::createChest(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	std::string item;
	split_line >> item;

	std::shared_ptr<Entity> content;

	if (item == "coin")
		content = createCoin(split_line);
	else if (item == "cake")
		content = createCake(split_line);
	else
		throw std::runtime_error("Scene::createChest: Bad content type");

	pos *= m_tilemap->getTileSize();

	std::cout << "Creating chest at " << pos.x << "," << pos.y << std::endl;

	m_entities.emplace_back(std::make_shared<Chest>(pos, m_tilemap, glm::ivec2(SCREEN_X, SCREEN_Y), m_tex_program, content));
}

std::shared_ptr<Coin> Scene::createCoin(std::istringstream& split_line)
{
	std::string size;
	split_line >> size;

	bool is_big;
	if (size == "big")
		is_big = true;
	else if (size == "small")
		is_big = false;
	else
		throw std::runtime_error("Scene::createCoin: Bad coin type");

	std::cout << "Creating coin" << std::endl;

	auto coin = std::make_shared<Coin>(glm::ivec2{0.0f, 0.0f}, m_tilemap, glm::ivec2(SCREEN_X, SCREEN_Y), m_tex_program, is_big);
	m_entities.push_back(coin);

	return coin;
}

std::shared_ptr<Cake> Scene::createCake(std::istringstream& split_line) 
{
	std::string size;
	split_line >> size;

	bool is_big;
	if (size == "big")
		is_big = true;
	else if (size == "small")
		is_big = false;
	else
		throw std::runtime_error("Scene::createCake: Bad cake type");

	std::cout << "Creating cake" << std::endl;

	auto cake = std::make_shared<Cake>(glm::ivec2{0.0f,0.0f}, m_tilemap, glm::ivec2(SCREEN_X, SCREEN_Y), m_tex_program, is_big);
	m_entities.push_back(cake);

	return cake;
}

void Scene::createVoid(std::istringstream& split_line) 
{
	glm::ivec2 upleft_corner_pos;
	glm::ivec2 collision_size;

	split_line >> upleft_corner_pos.x >> upleft_corner_pos.y >> collision_size.x >> collision_size.y;

	upleft_corner_pos *= m_tilemap->getTileSize();
	collision_size *= m_tilemap->getTileSize();

	m_entities.emplace_back(std::make_shared<Void>(upleft_corner_pos, collision_size));
}

void Scene::createPlatform(std::istringstream& split_line) 
{
	glm::ivec2 upleft_corner_pos;
	split_line >> upleft_corner_pos.x >> upleft_corner_pos.y;

	upleft_corner_pos *= m_tilemap->getTileSize();

	m_entities.emplace_back(std::make_shared<Platform>(upleft_corner_pos, m_tilemap->getTilesheet(), m_tilemap->getTileSize(), m_tex_program, m_tilemap));
}

void Scene::createBarrel(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();

	m_entities.emplace_back(std::make_shared<Barrel>(pos, m_tilemap, glm::ivec2(SCREEN_X, SCREEN_Y), m_tex_program));
}
