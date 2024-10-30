#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Camera.h"
#include "UI.h"


#define PLAYER_SPRITE_SIZE_X 32*4
#define PLAYER_SPRITE_SIZE_Y 48*4
#define PLAYER_COLLISION_SIZE_X 16*4
#define PLAYER_COLLISION_SIZE_Y 32*4 - 1

class Coin;
class Cake;

// Scene contains all the entities of our game.
// It is responsible for updating and render them.

enum class Screen
{
	StrartScreen, Tutorial, Level, Options, Credits
};

class Scene
{

public:
	Scene() = default;

	// Initializes the scene with an entity file and a level file
	void init();

	// Updates the scene
	void update(int delta_time);

	// Renders the scene
	void render();

	// Changes the screen, it will be updated in the next frame
	void setScreen(Screen new_screen);

private:
	void initShaders();

	// Actually changes the screen and takes care of the changes
	void changeScreen(Screen new_screen);

	// Reads from a file which entities will be on the level, with information on how to create them
	void readSceneFile(std::string const& path);

	// Creates a player and adds it to the scene
	void createPlayer(std::istringstream& split_line);

	// Creates a chest and adds it to the scene
	void createChest(std::istringstream& split_line);

	// Creates a coin and adds it to the scene
	[[nodiscard]] std::shared_ptr<Coin> createCoin(std::istringstream& split_line);

	// Creates a cake and adds it to the scene
	[[nodiscard]] std::shared_ptr<Cake> createCake(std::istringstream& split_line);

	// Creates a void and adds it to the scene
	void createVoid(std::istringstream& split_line);

	// Creates a platform and adds it to the scene
	void createPlatform(std::istringstream& split_line);

	// Creates a barrel and adds it to the scene
	void createBarrel(std::istringstream& split_line);

	// Creates a horse and adds it to the scene
	void createHorse(std::istringstream& split_line);

	// Creates a monkey and adds it to the scene
	void createMonkey(std::istringstream& split_line);

	// Creates a boss and adds it to the scene
	void createBoss(std::istringstream& split_line);

	// Creates a gem and adds it to the scene
	void createGem(std::istringstream& split_line);

	// Creates a rock and adds it to the scene
	void createRock(std::istringstream& split_line);

	// Creates a box and adds it to the scene
	void createBox(std::istringstream& split_line);

	// The tilemap
	std::shared_ptr<TileMap> m_tilemap;
	
	// The player
	std::shared_ptr<Player> m_player;

	// All entities in the scene, including the player
	std::vector<std::shared_ptr<Entity>> m_entities;
	
	// The texture shading program
	std::shared_ptr<ShaderProgram> m_tex_program;

	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<UI> m_ui;
	float m_current_time;

	Screen m_current_screen;
	Screen m_next_screen;

	glm::ivec2 const m_player_sprite_size {PLAYER_SPRITE_SIZE_X, PLAYER_SPRITE_SIZE_Y};
	glm::ivec2 const m_player_collision_size{PLAYER_COLLISION_SIZE_X, PLAYER_COLLISION_SIZE_Y-4};
};

#endif // _SCENE_INCLUDE
