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
    virtual void setEnabled (bool enabled) { m_enabled = enabled; }

    virtual void setSpawnPosition(glm::ivec2 position) { /*m_original_pos = position;*/ }

    void moveToOriginalPosition() { setPosition(m_original_pos); }

protected:
    // Updates the position according to the collision box of the solid
    void computeCollisionAgainstSolid(Entity* solid);

    // The spritesheet
    std::shared_ptr<Texture> m_spritesheet;
	
    // A pointer to the sprite
    std::shared_ptr<Sprite> m_sprite;

    // A pointer to the tilemap
    std::shared_ptr<TileMap> m_tilemap;

    // The coordinates of the midpoint in the base of the Entity
    glm::ivec2 m_pos;

    // (re)spawn position
    glm::ivec2 m_original_pos;
  
    // The x and y sizes of the collision box
    glm::ivec2 m_collision_box_size;
  
    // The velocity
    glm::vec2 m_vel;

    // The acceleration
    glm::vec2 m_acc {0.0f, 0.0f};

    constexpr static float S_GRAVITY = 0.006f;

    // True iff this entity can collide with the tilemap
    bool m_can_collide_with_tiles = true;

    // True iff this entity can collide with other entities. Disable for destroy or fading animations, for instance
    bool m_can_collide = true;

    // True iff this entity is enabled (is processed by the game)
    bool m_enabled = true;

    // True iff this entity is affected by gravity
    bool m_affected_by_gravity = false;

    // The bouncing coefficient
    static constexpr float S_BOUNCE_COEFF = -0.7f;

    // The bouncing minimum speed (to avoid vibrations)
    static constexpr float S_MIN_BOUNCE_SPEED = 0.25f;

    // True iff this entity is affected by bouncing
    bool m_bounces = false;

    // The x drag coefficient
    static constexpr float S_X_DRAG = 0.003f;

    // True iff the entity is affected by drag on the X axis
    bool m_affected_by_x_drag = false;

    // True iff the entity's "feet" are on the floor
    bool m_grounded = false;

    // The maximum |velocity| on the X axis
    float M_MAX_X_VELOCITY = 10.0f;

    // The maximum fall velocity (ie. maximum velocity on the Y axis)
    float M_MAX_FALL_VELOCITY = 10.0f;
};

#endif // _ENTITY_INCLUDE
