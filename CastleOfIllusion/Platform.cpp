#include "Platform.h"

void Platform::update(int delta_time)
{
    glm::ivec2 previous_pos = m_pos;
    Entity::update(delta_time);

    glm::ivec2 change_in_position = m_pos - previous_pos;
    bool pos_changed = (change_in_position != glm::ivec2(0, 0));
}