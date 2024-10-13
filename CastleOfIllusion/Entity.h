#ifndef _ENTITY_INCLUDE
#define _ENTITY_INCLUDE

#include "Texture.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Collision.h"
#include "EntityType.h"
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

    // Gets the entity's type
    virtual EntityType getType() const;

    // Gets the sprite's x and y sizes
    glm::ivec2 getSpriteSize() const;

    // Returns (minX,minY) and (maxX,maxY) of the collision box
    std::pair<glm::ivec2, glm::ivec2> getMinMaxCollisionCoords() const;

    // Called when the entity collides with something
    virtual void collideWithEntity(Collision collision) = 0;

    // Returns true iff this Entity is colliding with other
    bool operator&(Entity const& other) const;

    // Returns the collision info for this Entity (first) and the other (second)
    std::pair<Collision, Collision> operator|(Entity const& other) const;

protected:
    // The spritesheet
    std::shared_ptr<Texture> m_spritesheet;
	
    // A pointer to the sprite
    std::shared_ptr<Sprite> m_sprite;

    // A pointer to the tilemap
    std::shared_ptr<TileMap> m_tilemap;

    // The coordinates of the midpoint in the base of the Entity
    glm::ivec2 m_pos;
  
    // The x and y sizes of the collision box
    glm::ivec2 m_collision_box_size;

    // The type of this entity
    EntityType m_type = EntityType::Unknown;
  
    // The velocity
    glm::vec2 m_vel;

    constexpr static float S_GRAVITY = 0.003f;
};

#endif // _ENTITY_INCLUDE
