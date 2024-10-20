#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE

#include <GLFW/glfw3.h>
#include "Scene.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 640


// Game is a singleton (a class with a single instance) that represents our whole application

class Game
{
	
public:	
	// Initializes the game
	static void init();

	// Updates the game, and returns true if it should keep running
	static bool update(int delta_time);

	// Renders everything in the game
	static void render();

	// Called when a key is pressed
	static void keyPressed(int key);

	// Called when a key is released
	static void keyReleased(int key);

	// Called when the mouse moves
	static void mouseMove(int x, int y);

	// Called when a mouse button is pressed
	static void mousePress(int button);

	// Called when a mouse button is released
	static void mouseRelease(int button);

	// Gets if a certain key is pressed in the state
	static bool getKey(int key);

private:
	// Constructor defaults to private for the singleton pattern
	Game() = default;

	// Gets the only instance
	static Game& instance()
	{
		static Game G;
		return G;
	}

	// False iff the game should close
	bool m_is_playing = true;

	// Store key states so that we can have access at any time
	bool m_keys[GLFW_KEY_LAST+1];

	// The scene
	Scene m_scene;
};

#endif // _GAME_INCLUDE
