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
#include "Boss.h"
#include "Gem.h"
#include "Rock.h"
#include "Box.h"
#include "CameraPoint.h"

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
	m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

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
	TimedEvents::clearEvents();
	switch (new_screen)
	{
	case Screen::StrartScreen:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(true);
		break;
	}
	case Screen::Tutorial:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(false);

		m_tilemap.reset(TileMap::createTileMap("levels/tutorial.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));
		readSceneFile("levels/tutorial.entities");
		break;
	}
	case Screen::Level:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(false);

		m_tilemap.reset(TileMap::createTileMap("levels/normal.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));
		readSceneFile("levels/normal.entities");
		break;
	}
	case Screen::Options:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

		m_camera.reset(new Camera());
		m_camera->init(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_ui);
		m_camera->setStatic(true);
		break;
	}
	case Screen::Credits:
	{
		m_ui.reset(new UI());
		m_ui->init(m_tex_program, new_screen);
		m_ui->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });

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

	m_entities.clear();

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
		else if (word == "cameraPoint")
			createCameraPoint(split_line);
		else if (word == "platform")
			createPlatform(split_line);
		else if (word == "barrel")
			createBarrel(split_line);
		else if (word == "horse")
			createHorse(split_line);
		else if (word == "monkey")
			createMonkey(split_line);
		else if (word == "boss")
			createBoss(split_line);
		else if (word == "gem")
			createGem(split_line);
		else if (word == "rock")
			createRock(split_line);
		else if (word == "box")
			createBox(split_line);
		else if (word == "skip")
			continue;
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
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	m_player.reset(
		new Player(
			pos,
			m_tilemap, 
			m_ui, 
			glm::ivec2(SCREEN_X, SCREEN_Y), 
			m_player_sprite_size, 
			m_player_collision_size, 
			m_tex_program,
			m_camera));

	m_entities.push_back(m_player);

	m_camera->setPlayer(m_player);

	// Callbacks to change the scene
	m_player->setChangeScreenCallback([this](Screen scene_id) { setScreen(scene_id); });
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
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

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

	m_entities.emplace_back(std::make_shared<Void>(upleft_corner_pos, collision_size, m_tex_program));
}

void Scene::createCameraPoint(std::istringstream& split_line) 
{
	glm::ivec2 upleft_corner_pos;
	glm::ivec2 collision_size;
	glm::ivec2 player_spawn_point;
	int camera_offset;
	int id;

	split_line >> upleft_corner_pos.x >> upleft_corner_pos.y >> collision_size.x >> collision_size.y >> player_spawn_point.x >> player_spawn_point.y >> camera_offset >> id;

	upleft_corner_pos *= m_tilemap->getTileSize();
	collision_size *= m_tilemap->getTileSize();

	auto camera_point = std::make_shared<CameraPoint>(upleft_corner_pos, collision_size, m_camera, m_player, player_spawn_point, camera_offset, m_tex_program, id, m_boss);
	m_entities.emplace_back(camera_point);

	m_player->addReactivable(camera_point);
}

void Scene::createPlatform(std::istringstream& split_line) 
{
	glm::ivec2 upleft_corner_pos;
	split_line >> upleft_corner_pos.x >> upleft_corner_pos.y;

	upleft_corner_pos *= m_tilemap->getTileSize();
	upleft_corner_pos.x += 1;

	auto platform = std::make_shared<Platform>(upleft_corner_pos, m_tilemap->getTilesheet(), m_tilemap->getTileSize(), m_tex_program, m_tilemap);
	m_entities.emplace_back(platform);

	m_player->addReactivable(platform);
}

void Scene::createBarrel(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	auto barrel = std::make_shared<Barrel>(pos, m_tilemap, glm::ivec2(SCREEN_X, SCREEN_Y), m_tex_program);
	m_entities.emplace_back(barrel);

	m_player->addReactivable(barrel);
}

void Scene::createHorse(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

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
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

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
}

void Scene::createBoss(std::istringstream& split_line) 
{
	glm::ivec2 pos;
	glm::ivec2 other_pos;
	split_line >> pos.x >> pos.y >> other_pos.x >> other_pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);
	other_pos *= m_tilemap->getTileSize();
	other_pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	auto boss =
		std::make_shared<Boss>(
			pos, 
			other_pos, 
			glm::ivec2(2*m_tilemap->getTileSize(), 2*m_tilemap->getTileSize()),
			m_tilemap,
			m_tex_program,
			"images/Boss.png",
			"images/Blocks2.png",
			m_camera
			);

	m_boss = boss;

	auto tile_size = m_tilemap->getTileSize();

	for (int i = 1; i < 2; ++i)
	{
		auto rock_x = pos.x + m_tilemap->getTileSize() * (2 + 3 * i);
		std::string rock_str = std::to_string(rock_x / tile_size) + " " + std::to_string((pos.y - tile_size * 8) / tile_size);
		std::istringstream ss(rock_str);
		auto rock = createRock(ss);
		m_boss->addObject(rock);
	}

	auto box_x = pos.x + m_tilemap->getTileSize() * (3);
	std::string box_str = std::to_string(box_x / tile_size) + " " + std::to_string((pos.y - tile_size * 8) / tile_size);
	std::istringstream ss(box_str);
	auto box = createBox(ss);
	m_boss->addObject(box);

	std::string gem_str = std::to_string((pos.x + (other_pos.x - pos.x) / 2) / tile_size) + " " + std::to_string((pos.y - tile_size * 8) / tile_size);
	std::istringstream ss_gem(gem_str);
	auto gem = createGem(ss_gem);
	m_boss->setGem(gem);

	auto blocks = boss->getBlocks();

	// We want it rendered last -> put in first
	m_entities.push_back(boss);

	for (auto const& block : blocks)
		m_entities.push_back(block);
}

std::shared_ptr<Gem> Scene::createGem(std::istringstream& split_line)
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	auto gem = std::make_shared<Gem>(pos, m_tilemap, m_tex_program);
	m_entities.emplace_back(gem);

	return gem;
}

std::shared_ptr<Rock> Scene::createRock(std::istringstream& split_line)
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	auto rock = std::make_shared<Rock>(pos, m_tilemap, m_tex_program);
	m_entities.emplace_back(rock);

	return rock;
}

std::shared_ptr<Box> Scene::createBox(std::istringstream& split_line)
{
	glm::ivec2 pos;
	split_line >> pos.x >> pos.y;

	pos *= m_tilemap->getTileSize();
	pos += glm::ivec2(m_tilemap->getTileSize() / 2, 0.0f);

	auto box = std::make_shared<Box>(pos, m_tilemap, m_tex_program);
	m_entities.emplace_back(box);

	return box;
}
