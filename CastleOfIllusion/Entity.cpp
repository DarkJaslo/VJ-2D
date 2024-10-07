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
