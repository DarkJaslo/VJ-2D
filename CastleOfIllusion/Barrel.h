#ifndef _BARREL_INCLUDE
#define _BARREL_INCLUDE

#include "ThrowableTile.h"

class Barrel : public ThrowableTile
{
public:
	Barrel(glm::ivec2 pos,
		std::shared_ptr<TileMap> tilemap,
		glm::ivec2 tilemap_pos,
		std::shared_ptr<ShaderProgram> shader_program);

	virtual void collideWithEntity(Collision collision) override;
};

#endif // _BARREL_INCLUDE
