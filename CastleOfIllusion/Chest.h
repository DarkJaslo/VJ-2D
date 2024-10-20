#ifndef _CHEST_INCLUDE
#define _CHEST_INCLUDE

#include "ThrowableTile.h"

// Represents a chest, which is a throwable "tile" that drops some item
class Chest : public ThrowableTile 
{
public:
	Chest(glm::ivec2 pos,
		 std::shared_ptr<TileMap> tilemap,
		 glm::ivec2 tilemap_pos,
		 std::shared_ptr<ShaderProgram> shader_program,
		 std::shared_ptr<Entity> content);

	virtual void onDestroy() override;

private:
	// What is inside the chest
	std::shared_ptr<Entity> m_content;
};

#endif // _CHEST_INCLUDE