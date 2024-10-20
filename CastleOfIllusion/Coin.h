#ifndef _COIN_INCLUDE
#define _COIN_INCLUDE

#include "Entity.h"

class Coin : public Entity
{
public:
    Coin(glm::ivec2 const& pos,
        std::shared_ptr<TileMap> tilemap,
        glm::ivec2 const& tilemap_pos,
        std::shared_ptr<ShaderProgram> shader_program,
        bool is_big);

    virtual void update(int delta_time) override;

    // Called when the entity collides with something
    virtual void collideWithEntity(Collision collision);

    // Gets the entity's type
    virtual EntityType getType() const override { return EntityType::Coin; }

    // Returns the points gained from collecting this coin
    unsigned int getPoints() const;

private:
    // Points given to the player if the coin is big
    static constexpr unsigned int s_big_points = 2000;

    // Points given to the player if the coin is small
    static constexpr unsigned int s_small_points = 500;

    // Time the coin stays there until it disappears
    static constexpr int s_timeout = 3000;

    // The size of the sprite
    static constexpr int s_sprite_size = 16;

    // The time this coin has been alive
    int m_alive_time = 0;

    // True iff this coin is big, false if it is small
    bool m_is_big;
};

#endif