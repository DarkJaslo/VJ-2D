#ifndef _COLLISION_INCLUDE
#define _COLLISION_INCLUDE

#include <glm/glm.hpp>
#include <memory>
#include "EntityType.h"

// Stores information about a collision from the point of view of an entity (which is implicit)
// Meaning there will only be information on the other entity
class Collision
{
public:
    Collision(glm::vec2 vector, EntityType type)
        : vector(vector), type(type) { }

    Collision(Collision const& other) = default;
    Collision& operator=(Collision const& other) = default;
    Collision(Collision&& other) = default;
    Collision& operator=(Collision&& other) = default;

    // The vector from the position of the implicit Entity to the collided entity
    glm::vec2 vector;

    // The type of entity we collided with
    EntityType type;
};

#endif // _COLLISION_INCLUDE