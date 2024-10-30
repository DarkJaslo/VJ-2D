#ifndef _PLATFORM_INCLUDE
#define _PLATFORM_INCLUDE

#include "Sprite.h"
#include "Entity.h"
#include <vector>

class Platform : public Entity
{
public:
    // Assumes the position is the upleft corner
    Platform(glm::ivec2 pos, 
        std::shared_ptr<Texture> tilesheet, 
        int tile_size, 
        std::shared_ptr<ShaderProgram> shader_program, 
        std::shared_ptr<TileMap> tilemap);

    virtual void update(int delta_time) override;

    // Gets the entity's type
    virtual EntityType getType() const override { return EntityType::Platform; }

    // Called when the entity collides with something
    virtual void collideWithEntity(Collision collision) override;

    virtual void setEnabled(bool enabled) override;

private:
    // All entities on top of the platform each frame
    std::vector<Entity*> m_entities_on_top;

    // True iff the player has stoop on top of it; starts falling
    bool m_started_falling = false;
};

#endif
