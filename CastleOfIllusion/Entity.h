#ifndef _ENTITY_INCLUDE
#define _ENTITY_INCLUDE

#include "Texture.h"
#include "Sprite.h"
#include "TileMap.h"
#include <memory>

// Represents a game entity, that is, something that is dynamic, has a position, could move and
// follows most laws of the game, ie. moving when on top of a platform
class Entity
{
public:
    // vec2 are non const reference because they are only 8 bytes

    // Updates the entity
    virtual void update(int delta_time);

    // Renders the entity
    virtual void render();

    // Adds the given vector to the entity's position
    virtual void changePosition(glm::ivec2 change);

    // Sets the position of the entity
    virtual void setPosition(glm::ivec2 new_position);

    // Gets the entity's position
    virtual glm::ivec2 getPosition() const;

protected:
    // The spritesheet
    std::shared_ptr<Texture> m_spritesheet;
	
    // A pointer to the sprite
    std::shared_ptr<Sprite> m_sprite;

    // A pointer to the tilemap
    std::shared_ptr<TileMap> m_tilemap;

    // The position
    glm::ivec2 m_pos;

    // The velocity
    glm::vec2 m_vel;
};

#endif // _ENTITY_INCLUDE
