#include "Cake.h"
#include "TimedEvent.h"
#include <iostream>

Cake::Cake(glm::ivec2 pos,
    std::shared_ptr<TileMap> tilemap,
    glm::ivec2 tilemap_pos,
    std::shared_ptr<ShaderProgram> shader_program,
    bool is_big)
    : m_is_big(is_big)
{
    m_tilemap = tilemap;
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
    m_affected_by_gravity = true;
    m_bounces = true;
}

void Cake::update(int delta_time)
{
    if (!m_enabled)
        return;

    Entity::update(delta_time);
}

void Cake::collideWithEntity(Collision collision)
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

void Cake::setEnabled(bool enabled) 
{
    Entity::setEnabled(enabled);

    if (enabled) 
    {
        auto NoCollideAtStart = [this]()
        {
            if (!m_enabled)
                return;

            m_can_collide = true;
        };
        TimedEvents::pushEvent(std::make_unique<TimedEvent>(200, NoCollideAtStart));

        auto Flicker = [this]() 
        {
            m_sprite->startFlickering();
        };
        TimedEvents::pushEvent(std::make_unique<TimedEvent>(s_timeout/2, Flicker));

        auto Destroy = [this]()
        {
            if (!m_enabled)
                return;

            m_enabled = false;
            m_can_collide = false;
        };
        TimedEvents::pushEvent(std::make_unique<TimedEvent>(s_timeout, Destroy));
    }
}

unsigned int Cake::getPower() const
{
    if (m_is_big)
        return s_big_power;

    return s_small_power;
}