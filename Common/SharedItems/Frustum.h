#pragma once
#include "Renderer.h"

// Adaptation from: https://sites.google.com/site/letsmakeavoxelengine/home/frustum-culling
// And https://www.lighthouse3d.com/tutorials/view-frustum-culling/
// And https://www.youtube.com/watch?v=-ymmkfjMIeI

#include <glm/glm.hpp>

class Frustum {
public:
    enum { TOP = 0, BOTTOM, LEFT, RIGHT, NEAR, FAR };
    glm::vec4 planes[6];

    void Extract(const glm::mat4& viewProj);
    bool BoxInFrustum(const glm::vec3& center, float halfX, float halfY, float halfZ) const;
};

