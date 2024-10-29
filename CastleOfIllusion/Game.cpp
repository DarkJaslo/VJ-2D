#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"

void Game::init()
{
	instance().m_is_playing = true;
	glClearColor(0.53f, 0.77f, 1.0f, 1.0f);
	instance().m_scene.init("levels/normal.txt", "levels/normal.entities");
}

bool Game::update(int delta_time)
{
	instance().m_scene.update(delta_time);

	return instance().m_is_playing;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	instance().m_scene.render();
}

void Game::keyPressed(int key)
{
	if(key == GLFW_KEY_ESCAPE)
		instance().m_is_playing = false;

	instance().m_keys[key] = true;
}

void Game::keyReleased(int key)
{
	instance().m_keys[key] = false;
}

void Game::mouseMove(int x, int y) { }

void Game::mousePress(int button) { }

void Game::mouseRelease(int button) { }

bool Game::getKey(int key)
{
	return instance().m_keys[key];
}
