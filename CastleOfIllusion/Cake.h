#ifndef _CAKE_INCLUDE
#define _CAKE_INCLUDE

#include "Entity.h"

// Represents a cake, an item which is collected upon contact with the player. Can be found on chests.
// There are two types of cake, the small cake and the big cake. The only changes are the size, the sprite and the
// amount of power gained from collecting them.
class Cake : public Entity 
{
public:
	Cake(glm::ivec2 pos,
		std::shared_ptr<TileMap> tilemap,
		glm::ivec2 tilemap_pos,
		std::shared_ptr<ShaderProgram> shader_program,
		bool is_big);

	virtual void update(int delta_time) override;

	virtual void collideWithEntity(Collision collision);

	virtual EntityType getType() const override { return EntityType::Cake; }

	unsigned int getPower() const;

private:
	// Power gained by the player if the cake is big
	static constexpr unsigned int s_big_power = 3;

	// Power gained by the player if the cake is small
	static constexpr unsigned int s_small_power = 1;

	// Time the cake stays there until it disappears
	static constexpr int s_timeout = 3000;

	// The time this cake has been alive
	int m_alive_time = 0;

	// True iff this cake is big, false if it is small
	bool m_is_big;
};

#endif // _CAKE_INCLUDE
