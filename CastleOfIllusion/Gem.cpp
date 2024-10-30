#include "Gem.h"

Gem::Gem(glm::ivec2 const& pos,
    std::shared_ptr<TileMap> tilemap,
    std::shared_ptr<ShaderProgram> shader_program) 
{
    m_tilemap = tilemap;

    m_spritesheet.reset(new Texture());
    m_spritesheet->loadFromFile("images/Items.png", TEXTURE_PIXEL_FORMAT_RGBA);
    m_sprite.reset(Sprite::createSprite(glm::ivec2(tilemap->getTileSize(), 
        tilemap->getTileSize()) /* quad_size */,
        {0.125f, 0.5f},
        m_spritesheet,
        shader_program));

    // Orange gem
    m_sprite->setTextureCoordsOffset({ 0.125f, 0.5f });

    setPosition(pos);
    m_collision_box_size = glm::ivec2(tilemap->getTileSize(), tilemap->getTileSize());

    m_bounces = true;
    m_affected_by_gravity = false;
    m_affected_by_x_drag = false;
    m_can_collide = true;
    m_can_collide_with_tiles = true;
}

void Gem::update(int delta_time) 
{
    if (!m_enabled)
        return;

    Entity::update(delta_time);
}

void Gem::collideWithEntity(Collision collision)
{
    if (!m_enabled)
        return;

    switch (collision.entity->getType())
    {
    case EntityType::Player:
    {
        m_can_collide = false;
        m_enabled = false;
        break;
    }
    default:
        break;
    }
}

void Gem::setEnabled(bool enabled) 
{
    Entity::setEnabled(enabled);
    m_affected_by_gravity = enabled;
}