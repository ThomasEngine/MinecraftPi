#pragma once
#include "Renderer.h"
#include <glm/glm.hpp>

// Adaptation from: https://sites.google.com/site/letsmakeavoxelengine/home/frustum-culling
// mostly: https://stackoverflow.com/questions/12836967/extracting-view-frustum-planes-gribb-hartmann-method
// And https://www.youtube.com/watch?v=-ymmkfjMIeI


class Frustum {
public:
    enum { TOP = 0, BOTTOM, LEFT, RIGHT, NEAR, FAR };
    glm::vec4 planes[6];

    void Extract(const glm::mat4& viewProj);
    bool BoxInFrustum(const glm::vec3& center, float halfX, float halfY, float halfZ) const;
};

