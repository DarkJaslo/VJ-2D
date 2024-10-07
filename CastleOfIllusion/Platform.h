#ifndef _PLATFORM_INCLUDE
#define _PLATFORM_INCLUDE

#include "Sprite.h"
#include "Entity.h"
#include <vector>

class Platform : public Entity
{
public:
    virtual void update(int delta_time) override;

private:
    // The entities currently on top of the platform
    std::vector<std::shared_ptr<Entity>> m_entities_on_top;


};

#endif
