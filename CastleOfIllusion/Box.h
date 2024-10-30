#ifndef _BOX_INCLUDE
#define _BOX_INCLUDE

#include "ThrowableTile.h"

class Box : public ThrowableTile 
{
public:
	Box(glm::ivec2 pos,
		std::shared_ptr<TileMap> tilemap,
		std::shared_ptr<ShaderProgram> shader_program);

	virtual void collideWithEntity(Collision collision) override;

	virtual void setEnabled(bool enabled) override;

private:
	glm::ivec2 m_start_pos;
};

#endif