#ifndef _CAMERA_INCLUDE
#define _CAMERA_INCLUDE


#include <glm/glm.hpp>
#include "Player.h"

// The camera used to render the scene

class Camera
{
public:
    Camera(float width, float height, std::shared_ptr<Player> player, glm::vec2 update_speed);

	// Updates the camera
    void update(int delta_time);

	// Gets the camera's position (top left)
    void getPosition();
	
	// Gets the camera's projection matrix
	glm::mat4 getProjectionMatrix();    

private:
	// The game's player
	std::shared_ptr<Player> m_player;

	// The size
    glm::vec2 m_size;

	// The position (top left)
	glm::vec2 m_pos = glm::vec2(0,0);

	// The velocity at a certain moment
	glm::vec2 m_vel = glm::vec2(0,0);

	// The speed in which the camera catches the player when the player changes direction
	// Both values have to be greater than 1
	glm::vec2 m_update_speed;
};

#endif
