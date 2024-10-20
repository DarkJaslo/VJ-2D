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

    // Adds the given vector to the entity's velocity
    virtual void changeVelocity(glm::vec2 change);

    // Sets the position of the entity
    virtual void setPosition(glm::ivec2 new_position);

    // Sets the velocity of the entity
    virtual void setVelocity(glm::vec2 new_velocity);

    // Gets the entity's position
    virtual glm::ivec2 getPosition() const;
    
    // Gets the entity's velocity
    virtual glm::vec2 getVelocity() const;

    // Gets the entity's type
    virtual EntityType getType() const { return EntityType::Unknown; }

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

    // Returns true iff the entity can be processed for collision detection
    inline bool canCollide() const { return m_enabled && m_can_collide; }

    // Returns true iff the entity is enabled
    inline bool isEnabled() const { return m_enabled; }

    // Sets whether the entity can collide or not
    void setCollisions (bool can_collide) { m_can_collide = can_collide; }

    // Sets whether the entity is enabled or not
    void setEnabled (bool enabled) { m_enabled = enabled; }

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
  
    // The velocity
    glm::vec2 m_vel;

    constexpr static float S_GRAVITY = 0.006f;

    // True iff this entity can collide with other entities. Disable for destroy or fading animations, for instance
    bool m_can_collide = true;

    // True iff this entity is enabled (is processed by the game)
    bool m_enabled = true;

};

#endif // _ENTITY_INCLUDE
