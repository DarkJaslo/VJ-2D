#include "Platform.h"

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
    m_entities_on_top.push_back(collision.entity);
}