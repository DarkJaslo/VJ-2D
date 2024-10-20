#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Camera.h"

class Coin;
class Cake;

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

	// Reads from a file which entities will be on the level, with information on how to create them
	void readSceneFile(std::string&& path);

	// Creates a chest and adds it to the scene
	void createChest(std::istringstream& split_line);

	// Creates a coin and adds it to the scene
	[[nodiscard]] std::shared_ptr<Coin> createCoin(std::istringstream& split_line);

	// Creates a cake and adds it to the scene
	[[nodiscard]] std::shared_ptr<Cake> createCake(std::istringstream& split_line);

	// The tilemap
	std::shared_ptr<TileMap> m_tilemap;
	
	// The player
	std::shared_ptr<Player> m_player;

	// All entities in the scene, including the player
	std::vector<std::shared_ptr<Entity>> m_entities;
	
	// The texture shading program
	std::shared_ptr<ShaderProgram> m_tex_program;
	std::shared_ptr<Camera> m_camera;
	float m_current_time;
};

#endif // _SCENE_INCLUDE
