#pragma once

#include <glm/vec2.hpp>

struct AABB {
    glm::vec2 pos;
    glm::vec2 size;
};
/** 
 * Check for the aabb collision
 * @param r1 first rectangle
 * @param r2 second rectangle
 * @returns Whether or not they collide
 */
bool collision(AABB r1, AABB r2);
