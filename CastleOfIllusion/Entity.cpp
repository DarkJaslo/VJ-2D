#include "Entity.h"

#include <iostream>
#include <algorithm>

void Entity::update(int delta_time)
{
    if (!m_enabled)
        return;

    m_sprite->update(delta_time);

    // Update Y position
    if (m_affected_by_gravity) 
    {
        m_vel.y += S_GRAVITY * static_cast<float>(delta_time);
        m_pos.y += static_cast<int>(m_vel.y * static_cast<float>(delta_time));
    }

    if (m_can_collide) 
    {
        auto y_collision = m_tilemap->yCollision(getMinMaxCollisionCoords().first, m_collision_box_size, m_vel);
        if (y_collision)
        {
            m_pos.y = y_collision->y + m_collision_box_size.y;

            if (m_bounces)
            {
                m_vel.y *= S_BOUNCE_COEFF;
                std::cout << "Vel y: " << m_vel.y << "\n";
                if (abs(m_vel.y) <= S_MIN_BOUNCE_SPEED)
                    m_vel.y = 0.0f;
            }
            else
                m_vel.y = 0.0f;
        }
    }

    // Saves work if the object is not moving (common case)
    if (m_vel == glm::vec2(0.0f, 0.0f))
    {
        m_sprite->setPosition(m_pos);
        return;
    }

    // Update X position
    if (m_affected_by_x_drag) 
    {
        if (m_vel.x > 0) 
            m_vel.x = std::max(m_vel.x - S_X_DRAG * static_cast<float>(delta_time), 0.0f);
        else if (m_vel.x < 0)
            m_vel.x = std::min(m_vel.x + S_X_DRAG * static_cast<float>(delta_time), 0.0f);
    }

    m_pos.x += m_vel.x * static_cast<float>(delta_time);

    if (m_can_collide) 
    {
        auto x_collision = m_tilemap->xCollision(getMinMaxCollisionCoords().first, m_collision_box_size, m_vel);
        if (x_collision)
        {
            m_pos.x = x_collision->x + m_collision_box_size.x / 2;

            if (m_bounces)
            {
                m_vel.x *= S_BOUNCE_COEFF;
                if (abs(m_vel.x) <= S_MIN_BOUNCE_SPEED)
                    m_vel.x = 0.0f;
            }
        }
    }

    m_sprite->setPosition(m_pos);
}

void Entity::render()
{
    if (m_enabled)
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

glm::ivec2 Entity::getSpriteSize() const
{
    if (!m_sprite)
        return glm::ivec2(0,0);

    return m_sprite->getQuadSize();
}

std::pair<glm::ivec2, glm::ivec2> Entity::getMinMaxCollisionCoords() const 
{
    glm::vec2 min(m_pos.x - m_collision_box_size.x / 2.f, m_pos.y - m_collision_box_size.y);
    glm::vec2 max(m_pos.x + m_collision_box_size.x / 2.f, m_pos.y);

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
    return std::make_pair(Collision(other.m_pos - m_pos, const_cast<Entity*>(&other)), 
                          Collision(m_pos - other.m_pos, const_cast<Entity*>(this)));
}
