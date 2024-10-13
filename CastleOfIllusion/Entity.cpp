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

void Entity::changeVelocity(glm::vec2 change)
{
    m_vel += change;
}

void Entity::setPosition(glm::ivec2 new_position)
{
    m_pos = new_position;
    m_sprite->setPosition(new_position);
}

void Entity::setVelocity(glm::vec2 new_velocity)
{
    m_vel = new_velocity;
}

glm::ivec2 Entity::getPosition() const
{
    return m_pos;
}

glm::vec2 Entity::getVelocity() const
{
    return m_vel;
}
