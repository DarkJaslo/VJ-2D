#ifndef _COLLISION_INCLUDE
#define _COLLISION_INCLUDE

#include <glm/glm.hpp>
#include <memory>

class Entity;

// Stores information about a collision from the point of view of an entity (which is implicit)
// Meaning there will only be information on the other entity
class Collision
{
public:
    Collision(glm::vec2 vector, Entity* entity)
        : vector(vector), entity(entity) { }

    Collision(Collision const& other) = default;
    Collision& operator=(Collision const& other) = default;
    Collision(Collision&& other) = default;
    Collision& operator=(Collision&& other) = default;

    // The vector from the position of the implicit Entity to the collided entity
    glm::ivec2 vector;

    // The entity we collided with
    Entity* entity;
};

#endif // _COLLISION_INCLUDE