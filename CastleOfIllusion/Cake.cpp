#include "Cake.h"
#include <iostream>

Cake::Cake(glm::ivec2 pos,
    std::shared_ptr<TileMap> tilemap,
    glm::ivec2 tilemap_pos,
    std::shared_ptr<ShaderProgram> shader_program,
    bool is_big)
    : m_is_big(is_big)
{
    m_spritesheet.reset(new Texture());
    m_spritesheet->loadFromFile("images/Items.png", TEXTURE_PIXEL_FORMAT_RGBA);
    glm::vec2 size_in_texture = glm::vec2(0.125f, 0.5f);
    m_sprite.reset(Sprite::createSprite(glm::ivec2(tilemap->getTileSize(), tilemap->getTileSize()) /* quad_size */,
        size_in_texture,
        m_spritesheet,
        shader_program));

    glm::vec2 tex_coords = is_big ? glm::vec2(0.125f, 0.0f) : glm::vec2(0.0f, 0.0f);
    m_sprite->setTextureCoordsOffset(tex_coords);

    setPosition(pos);
    m_collision_box_size = glm::ivec2(tilemap->getTileSize(), tilemap->getTileSize());

    m_enabled = false;
    m_can_collide = false;
}

void Cake::update(int delta_time)
{
    if (!m_enabled)
        return;

    Entity::update(delta_time);

    m_alive_time += delta_time;
    if (m_alive_time >= s_timeout)
    {
        // Disappear
    }
}

void Cake::collideWithEntity(Collision collision)
{
    if (!m_enabled)
        return;

    switch (collision.entity->getType())
    {
    case EntityType::Player:
    {
        // Disappear, stop being collectable
        m_can_collide = false;
        m_enabled = false;

        // Collect animation
        // Collect sound
        break;
    }
    default:
        break;
    }
}

unsigned int Cake::getPower() const
{
    if (m_is_big)
        return s_big_power;

    return s_small_power;
}