#pragma once

#include <glm/glm.hpp>

struct Player
{
    glm::vec3 position{ 8.0f, 30.0f, 20.0f };
    glm::vec3 velocity{ 0.0f };

    bool onGround = false;

    float width = 0.6f;
    float height = 1.8f;
    float eyeHeight = 1.6f;
};