#include "Entity.h"
#include "ThrowableTile.h"

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
        // Only apply max velocity when falling, gravity will do the other case for us
        if (m_vel.y > 0)
        {
            float new_vel = m_vel.y + S_GRAVITY * static_cast<float>(delta_time);
            if (abs(new_vel) < M_MAX_FALL_VELOCITY)
                m_vel.y = new_vel;
        }
        else
            m_vel.y += S_GRAVITY * static_cast<float>(delta_time);
    }
    m_pos.y += static_cast<int>(m_vel.y * static_cast<float>(delta_time));

    if (m_can_collide_with_tiles)
    {
        auto y_collision = m_tilemap->yCollision(getMinMaxCollisionCoords().first, m_collision_box_size, m_vel);
        if (y_collision)
        {
            m_pos.y = y_collision->y + m_collision_box_size.y;
            m_acc.y = 0.0f;

            if (m_bounces)
            {
                m_vel.y *= S_BOUNCE_COEFF;
                std::cout << "Vel y: " << m_vel.y << "\n";
                if (abs(m_vel.y) <= S_MIN_BOUNCE_SPEED)
                    m_vel.y = 0.0f;
            }
            else
                m_vel.y = 0.0f;

            // This is a bit of a hack, but it is way more comfortable to check for this here since
            // we progressively update the position
            if (auto throwable = dynamic_cast<ThrowableTile*>(this)) 
            {
                if (throwable->isBeingThrown()) 
                {
                    if (throwable->isDestroyedOnImpact())
                        throwable->onDestroy();
                    else
                        throwable->m_thrown = false;
                }
            }   
        }

        m_grounded = m_tilemap->isGrounded(getMinMaxCollisionCoords().first, m_collision_box_size);
    }

    // Update X position
    float new_vel = m_vel.x + m_acc.x * static_cast<float>(delta_time);
    if (abs(new_vel) < M_MAX_X_VELOCITY)
        m_vel.x = new_vel;

    if (m_affected_by_x_drag) 
    {
        if (m_vel.x > 0) 
            m_vel.x = std::max(m_vel.x - S_X_DRAG * static_cast<float>(delta_time), 0.0f);
        else if (m_vel.x < 0)
            m_vel.x = std::min(m_vel.x + S_X_DRAG * static_cast<float>(delta_time), 0.0f);
    }

    m_pos.x += m_vel.x * static_cast<float>(delta_time);

    if (m_can_collide_with_tiles)
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

            // This is a bit of a hack, but it is way more comfortable to check for this here since
            // we progressively update the position
            if (auto throwable = dynamic_cast<ThrowableTile*>(this))
            {
                if (throwable->isBeingThrown() && throwable->isDestroyedOnImpact())
                    throwable->onDestroy();
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

void Entity::computeCollisionAgainstSolid(Entity* solid)
{
    // Physically collide with it: in this case it's a bit more difficult because we already have the
    // updated X,Y coordinates of the player instead of being able to update them one by one.
    //
    // However, if we assume certain things about the game, there is a solution that works
    auto [min, max] = solid->getMinMaxCollisionCoords();

    // If the amount of the player that is inside the box exceeds these values, then the player is 
    // actually not inside in that axis
    int const Y_MAX_MARGIN = m_collision_box_size.y / 2;
    int const X_MAX_MARGIN = m_collision_box_size.x / 4;

    auto player_max_y = m_pos.y;
    auto player_min_y = m_pos.y - m_collision_box_size.y;
    auto player_max_x = m_pos.x + m_collision_box_size.x / 2.0f;
    auto player_min_x = m_pos.x - m_collision_box_size.x / 2.0f;

    int y_inside_up = player_max_y - min.y;
    int y_inside_down = max.y - player_min_y;
    int x_inside_left = player_max_x - min.x;
    int x_inside_right = max.x - player_min_x + 1;

    bool actually_inside_y = y_inside_up < Y_MAX_MARGIN || y_inside_down < Y_MAX_MARGIN;
    bool actually_inside_x = x_inside_left < X_MAX_MARGIN || x_inside_right < X_MAX_MARGIN;

    auto CorrectX = [&]()
    {
        if (x_inside_left < X_MAX_MARGIN) 
        {
            m_pos.x -= x_inside_left;

            // Only if we are actually walking against it
            if (m_vel.x > 0)
                m_vel.x = 0.0f;
        }
        else 
        {
            m_pos.x += x_inside_right;

            // Only if we are actually walking against it
            if (m_vel.x < 0)
                m_vel.x = 0.0f;
        }
    };

    auto CorrectY = [&]()
    {
        if (y_inside_up < Y_MAX_MARGIN)
        {
            m_pos.y -= y_inside_up;
            m_grounded = true;

            // This is a bit of a hack, but it is way more comfortable to check for this here since
            // we progressively update the position
            if (auto throwable = dynamic_cast<ThrowableTile*>(this))
            {
                if (throwable->isBeingThrown())
                {
                    if (throwable->isDestroyedOnImpact())
                        throwable->onDestroy();
                    else
                        throwable->m_thrown = false;
                }
            }
        }
        else
            m_pos.y += y_inside_down;

        m_vel.y = 0.0f;
    };

    if (actually_inside_x && actually_inside_y)
    {
        // Inside in both axes, correct the smallest ("less inside")
        int x_inside = x_inside_left < X_MAX_MARGIN ? x_inside_left : x_inside_right;
        int y_inside = y_inside_up < Y_MAX_MARGIN ? y_inside_up : y_inside_down;

        if (y_inside_up < x_inside)
            CorrectY();
        else
            CorrectX();
    }
    else if (actually_inside_y)
        CorrectY();
    else if (actually_inside_x)
        CorrectX();
}
