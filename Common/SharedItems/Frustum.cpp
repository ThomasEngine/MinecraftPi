#include "Frustum.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <algorithm>
#include <array>
#include <cmath>

void Frustum::Extract(const glm::mat4& viewProj)
{
    planes[LEFT] = glm::normalize(glm::row(viewProj, 3) + glm::row(viewProj, 0));
    planes[RIGHT] = glm::normalize(glm::row(viewProj, 3) - glm::row(viewProj, 0));
    planes[BOTTOM] = glm::normalize(glm::row(viewProj, 3) + glm::row(viewProj, 1));
    planes[TOP] = glm::normalize(glm::row(viewProj, 3) - glm::row(viewProj, 1));
    planes[NEAR] = glm::normalize(glm::row(viewProj, 3) + glm::row(viewProj, 2));
    planes[FAR] = glm::normalize(glm::row(viewProj, 3) - glm::row(viewProj, 2));
}

bool Frustum::BoxInFrustum(const glm::vec3& center, float halfX, float halfY, float halfZ) const
{
	// AABB COLLISION TEST AGAINST ALL SIX PLANES
    for (int i = 0; i < 6; ++i) {
        const glm::vec4& p = planes[i];
        float x = p.x >= 0 ? center.x + halfX : center.x - halfX;
        float y = p.y >= 0 ? center.y + halfY : center.y - halfY;
        float z = p.z >= 0 ? center.z + halfZ : center.z - halfZ;
        // If the positive vertex is outside, the box is outside
        if (p.x * x + p.y * y + p.z * z + p.w < 0)
            return false;
    }
    return true;
}
