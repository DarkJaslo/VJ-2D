#include "Entity.h"

void Entity::update(int delta_time)
{
    m_sprite->update(delta_time);
}

void Entity::render()
{
    m_sprite->render();
}

void Entity::changePosition(glm::ivec2 change)
{
    m_pos += change; 
    m_sprite->setPosition(m_pos);
}

void Entity::setPosition(glm::ivec2 new_position)
{
    m_pos = new_position;
    m_sprite->setPosition(new_position);
}

glm::ivec2 Entity::getPosition() const
{
    return m_pos;
}

EntityType Entity::getType() const 
{
    return m_type;
}

glm::ivec2 Entity::getSpriteSize() const
{
    if (!m_sprite)
        return glm::ivec2(0,0);

    return m_sprite->getQuadSize();
}

std::pair<glm::ivec2, glm::ivec2> Entity::getMinMaxCollisionCoords() const 
{
    glm::vec2 min(m_pos.x - m_collision_box_size.x / 2.f, m_pos.y);
    glm::vec2 max(m_pos.x + m_collision_box_size.x / 2.f, m_pos.y + m_collision_box_size.y);

    return std::make_pair(min, max);
}

bool Entity::operator&(Entity const& other) const
{
    auto const& [min, max] = getMinMaxCollisionCoords();
    auto const& [other_min, other_max] = other.getMinMaxCollisionCoords();

    return max.x >= other_min.x && min.x <= other_max.x && max.y >= other_min.y && min.y <= other_max.y;
}

std::pair<Collision, Collision> Entity::operator|(Entity const& other) const
{
    return std::make_pair(Collision(other.m_pos - m_pos, other.getType()), 
                          Collision(m_pos - other.m_pos, m_type));
}
