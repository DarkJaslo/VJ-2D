#include <iostream>
#include <cmath>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 25
#define SCREEN_Y 15

// coordinates of the tile where the player appears 
#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 8


Scene::Scene()
{
	m_tilemap.reset();
	m_player.reset();
	m_tex_program.reset(new ShaderProgram());
	m_camera.reset();
}

void Scene::init()
{
	initShaders();
	m_tilemap.reset(TileMap::createTileMap("levels/testSimple.txt", glm::vec2(SCREEN_X, SCREEN_Y), *m_tex_program));

	m_player.reset(new Player(glm::vec2(INIT_PLAYER_X_TILES * m_tilemap->getTileSize(), INIT_PLAYER_Y_TILES * m_tilemap->getTileSize()), 
		                      m_tilemap, 
		                      glm::ivec2(SCREEN_X, SCREEN_Y), 
		                      m_tex_program));
  	m_camera.reset(new Camera(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), m_player));

	m_entities.push_back(m_player);

	m_current_time = 0.0f;
}

void Scene::update(int delta_time)
{
	m_current_time += delta_time;
	m_player->update(delta_time);

	// Check collisions between entities (each pair once)
	for (std::size_t i = 0; i < m_entities.size(); ++i)
	{
		// TODO: skip non-visible
		for (std::size_t j = i+1; j < m_entities.size(); ++j)
		{
			// TODO: skip non-visible
			if (*m_entities[i] & *m_entities[j])
			{
				auto&& [i_collision, j_collision] = *m_entities[i] | *m_entities[j];
				m_entities[i]->collideWithEntity(i_collision);
				m_entities[j]->collideWithEntity(j_collision);
			}
		}
	}

	m_camera->update(delta_time);
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
	m_player->render();
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
