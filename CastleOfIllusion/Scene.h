#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"

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

	// The tilemap
	std::shared_ptr<TileMap> m_tilemap;
	
	// The player
	std::shared_ptr<Player> m_player;

	// All entities in the scene, including the player
	std::vector<std::shared_ptr<Entity>> m_entities;
	
	// The texture shading program
	std::shared_ptr<ShaderProgram> m_tex_program;
	float m_current_time;
	glm::mat4 m_projection_matrix;

};


#endif // _SCENE_INCLUDE

