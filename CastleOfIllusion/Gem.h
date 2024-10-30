#ifndef _GEM_INCLUDE
#define _GEM_INCLUDE

#include "Entity.h"

class Gem : public Entity 
{
public:
    Gem(glm::ivec2 const& pos,
        std::shared_ptr<TileMap> tilemap,
        std::shared_ptr<ShaderProgram> shader_program);

    virtual void update(int delta_time) override;

    // Called when the entity collides with something
    virtual void collideWithEntity(Collision collision);

    // Gets the entity's type
    virtual EntityType getType() const override { return EntityType::Gem; }

    // Sets whether the gem is enabled or not
    virtual void setEnabled(bool enabled) override;

private:
};

#endif
