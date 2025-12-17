#pragma once
#include <glm/glm.hpp>
#include <vector>


#ifdef WINDOWS_BUILD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <GLES3/gl3.h>
#endif



struct FaceVertex {
    glm::vec3 pos;
    glm::vec2 tex;
    float cellX;
    float cellY;
    float light;
    float ao = 1;
};

class Mesh {
public:
    Mesh(const std::vector<FaceVertex>& verts, const std::vector<unsigned int>& inds);
	Mesh() {}

    // Vertex data
	std::vector<FaceVertex> vertices;
	std::vector<unsigned int> indices;
    GLsizei indexCount = 0;

    // GPU
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
};

enum FaceDirection {
    FACE_BACK = 0,
    FACE_FRONT = 1,
    FACE_BOTTOM = 2,
    FACE_TOP = 3,
    FACE_LEFT = 4,
    FACE_RIGHT = 5
};
