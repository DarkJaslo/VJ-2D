#include "Platform.h"
#include "TimedEvent.h"

Platform::Platform(glm::ivec2 pos, std::shared_ptr<Texture> tilesheet, int tile_size, std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<TileMap> tilemap)
{
    if (tile_size % 2 != 0)
        throw std::runtime_error("Tile size is not a multiple of 2!");

    m_tilemap = tilemap;

    m_pos = pos + glm::ivec2(tile_size + tile_size / 2, tile_size);
    glm::ivec2 const size { 3 * tile_size, tile_size };
    m_collision_box_size = size;

    m_affected_by_gravity = false;
    m_affected_by_x_drag = false;
    m_bounces = false;
    
    m_sprite.reset(Sprite::createSprite(size, { 3.0f / 16.0f, 1.0f / 16.0f } /* quad_size */, tilesheet, shader_program));
    m_sprite->setTextureCoordsOffset({ 3.0f/16.0f, 2.0f/16.0f });
    m_sprite->setPosition(m_pos);
}

void Platform::update(int delta_time)
{
    glm::ivec2 previous_pos = m_pos;
    Entity::update(delta_time);

    glm::ivec2 change_in_position = m_pos - previous_pos;
    bool pos_changed = (change_in_position != glm::ivec2(0, 0));

    if (pos_changed) 
    {
        for (auto entity : m_entities_on_top) 
        {
            entity->changePosition(change_in_position);
        }
    }

    m_entities_on_top.clear();
}

void Platform::collideWithEntity(Collision collision)
{
    switch (collision.entity->getType()) 
    {
    case EntityType::Player: 
    {
        if (!m_started_falling) 
        {
            m_started_falling;
            TimedEvents::pushEvent(std::make_unique<TimedEvent>(333, [this]() { m_affected_by_gravity = true; }));
        }
    }
    // Fall-through
    case EntityType::Enemy:
    case EntityType::ThrowableTile:
    case EntityType::Coin:
    case EntityType::Cake:
    {
        m_entities_on_top.push_back(collision.entity);
        break;
    }
    case EntityType::Void:
        // Reappear or something, disable gravity
        break;
    default:
        break;
    }
}