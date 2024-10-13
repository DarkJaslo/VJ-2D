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

};

#endif
