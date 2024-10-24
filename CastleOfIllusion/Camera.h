#ifndef _CAMERA_INCLUDE
#define _CAMERA_INCLUDE


#include <glm/glm.hpp>
#include "Player.h"
#include "UI.h"

// The camera used to render the scene

class Camera
{
public:
    Camera(float width, float height, std::shared_ptr<Player> player, std::shared_ptr<UI> ui);

	// Updates the camera
    void update(int delta_time);

	// Gets the camera's size
    glm::vec2 getSize() const;

	// Gets the camera's position (top left)
    glm::vec2 getPosition() const;
	
	// Gets the camera's projection matrix
	glm::mat4 getProjectionMatrix() const;    

private:
	// The game's player
	std::shared_ptr<Player> m_player;

	// The game's UI
	std::shared_ptr<UI> m_ui;

	// The size of the window the camera renders
    glm::vec2 m_size;

	// The position (top left)
	glm::vec2 m_pos{0,0};

	// The velocity at a certain moment
	glm::vec2 m_vel{0,0};

	// The speed in which the camera catches the player when the player changes direction
	// Both values have to be greater than 1
	glm::vec2 m_update_speed;
};

#endif
