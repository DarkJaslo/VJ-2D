#ifndef _PLATFORM_INCLUDE
#define _PLATFORM_INCLUDE

#include "Sprite.h"
#include "Entity.h"
#include <vector>

class Platform : public Entity
{
public:
    virtual void update(int delta_time) override;

    // Gets the entity's type
    virtual EntityType getType() const override { return EntityType::Platform; }

    // Called when the entity collides with something
    virtual void collideWithEntity(Collision collision) override;

private:
    std::vector<Entity*> m_entities_on_top;
};

#endif
