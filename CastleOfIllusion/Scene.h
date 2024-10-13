#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Camera.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	std::shared_ptr<TileMap> m_tilemap;
	std::shared_ptr<Player> m_player;
	std::shared_ptr<ShaderProgram> m_tex_program;
	std::shared_ptr<Camera> m_camera;
	float m_current_time;
};


#endif // _SCENE_INCLUDE

