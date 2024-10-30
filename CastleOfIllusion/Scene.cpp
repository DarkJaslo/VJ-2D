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
#include "Enemy.h"

// Tilemap top left screen position
#define SCREEN_X 0
#define SCREEN_Y 0

void Scene::init()
{
	m_tilemap.reset();
	m_player.reset();
	m_tex_program.reset(new ShaderProgram());

	initShaders();

	m_ui.reset(new UI());
	m_ui->init(m_tex_program, Screen::StrartScreen);
	m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

	m_camera.reset(new Camera());
	m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
	m_camera->setStatic(true);

	m_current_screen = Screen::StrartScreen;
	m_next_screen = Screen::StrartScreen;


	m_current_time = 0.0f;
}

void Scene::update(int delta_time)
{
	m_current_time += delta_time;

	// Updates scheduled events, if any
	TimedEvents::updateEvents(delta_time);

	// Change screen if necessary
	if (m_current_screen != m_next_screen)
	{
		changeScreen(m_next_screen);
	}


	switch (m_current_screen)
	{
	case Screen::StrartScreen:
	{
		break;
	}
	case Screen::Tutorial:
	case Screen::Level:
	{
		// This includes the player, which is first of all
		for (auto& entity : m_entities)
		{
			entity->update(delta_time);
		}

		// Check collisions between entities (each pair once)
		for (std::size_t i = 0; i < m_entities.size(); ++i)
		{
			if (!m_entities[i]->canCollide())
				continue;

			for (std::size_t j = i + 1; j < m_entities.size(); ++j)
			{
				if (!m_entities[j]->canCollide())
					continue;

				if (*m_entities[i] & *m_entities[j])
				{
					//std::cout << "Detected collision between entities" << std::endl;

					auto&& [i_collision, j_collision] = *m_entities[i] | *m_entities[j];
					m_entities[i]->collideWithEntity(i_collision);
					m_entities[j]->collideWithEntity(j_collision);
				}
			}
		}
		break;
	}
	case Screen::Options:
	{
		break;
	}
	case Screen::Credits:
	{
		break;
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



	switch (m_current_screen)
	{
	case Screen::StrartScreen:
	{
		break;
	}
	case Screen::Tutorial:
	case Screen::Level:
	{
		m_tilemap->render();

		// The player is rendered the last
		for (int i = m_entities.size() - 1; i >= 0; --i)
		{
			if (m_entities[i]->isEnabled())
				m_entities[i]->render();
		}
		break;
	}
	case Screen::Options:
	{
		break;
	}
	case Screen::Credits:
	{
		break;
	}

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

void Scene::setScreen(Screen new_screen)
{
	m_next_screen = new_screen;
}

void Scene::changeScreen(Screen new_screen)
{
	m_current_screen = new_screen;
	switch (new_screen)
	{
	case Screen::StrartScreen:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(true);
		break;
	}
	case Screen::Tutorial:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(false);

		m_tilemap.reset(TileMap::createTileMap("levels/level1.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));
		readSceneFile("levels/testTiles.entities");
		break;
	}
	case Screen::Level:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(false);

		m_tilemap.reset(TileMap::createTileMap("levels/level1.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));
		readSceneFile("levels/testTiles.entities");
		break;
	}
	case Screen::Options:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(true);
		break;
	}
	case Screen::Credits:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(true);
		break;
	}
			
	}
}

void Scene::readSceneFile(std::string const& path)
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

		if (word == "player")
			createPlayer(split_line);
		else if (word == "chest")
			createChest(split_line);
		else if (word == "void")
			createVoid(split_line);
		else if (word == "platform")
			createPlatform(split_line);
		else if (word == "barrel")
			createBarrel(split_line);
		else if (word == "horse")
			createHorse(split_line);
		else if (word == "monkey")
			createMonkey(split_line);
		else if (word != "") // Empty lines cause this
		{
			std::cerr << "Scene::readSceneFile: wrong word: " << word << std::endl;
			throw std::runtime_error("");
		}
	}
}

void Scene::createPlayer(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();

	std::cout << "Creating player at " << pos.x << "," << pos.y << std::endl;

	m_player.reset(
		new Player(
			pos,
			m_tilemap, 
			m_ui, 
			glm::ivec2(SCREEN_X, SCREEN_Y), 
			m_player_sprite_size, 
			m_player_collision_size, 
			m_tex_program));

	m_entities.push_back(m_player);

	m_camera->setPlayer(m_player);

	// Callbacks to change the scene
	m_player->setChangeSceneCallback([this](Screen scene_id) { setScreen(scene_id); });
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

void Scene::createHorse(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();

	m_entities.emplace_back(
		std::make_shared<SpringHorse>(
			pos, 
			m_tilemap, 
			m_tex_program, 
			"images/Horse.png", 
			glm::vec2(0.25f, 1.0f), 
			m_camera,
			m_player));
}

void Scene::createMonkey(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();

	auto monkey = std::make_shared<CymbalMonkey>(
		pos,
		m_tilemap,
		m_tex_program,
		"images/Monkey.png",
		glm::vec2(0.25f, 1.0f),
		m_camera,
		m_player);

	m_entities.emplace_back(monkey);
	m_entities.emplace_back(monkey->getProjectile());

	std::cout << "Creating monkey at " << monkey->getPosition().x << "," << monkey->getPosition().y << std::endl;
}

