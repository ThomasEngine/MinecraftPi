#pragma once
#include <vector>
#include <glm/glm.hpp>

#ifdef WINDOWS_BUILD
//include glad and glfw for Windows build
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SharedItems/gui.h>
#include <imgui/imgui.h>
#else
#include <GLES2/gl2.h>
#endif

#ifdef Raspberry_BUILD
#include <GLES3/gl3.h>
#endif

// Error handling macros
#ifdef _DEBUG
#define ASSERT(x) if (!(x)) __debugbreak()
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif


void GLClearError();
void GLCheckError();
bool GLLogCall(const char* function, const char* file, int line);


struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLsizei indexCount = 0;
};

struct FaceVertex {
    glm::vec3 pos;
    glm::vec2 tex;
    unsigned int blockType;  // Block type
    uint8_t face;       // Face index
    uint8_t facesT;
};

class Shader;
class Texture;
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialize with an already created GLFW window (GL context current)
    bool init();
    void shutdown();

    void beginFrame();
    void endFrame();

    // Shader creation

    // Mesh helpers
    Mesh createCubeMesh();
    Mesh uploadMesh(const std::vector<float>& vertexData, const std::vector<unsigned int>& indices);
    Mesh uploadMesh(const std::vector<FaceVertex>& vertexData, const std::vector<unsigned int>& indices);
    void destroyMesh(Mesh& m);

    // Draw
    void drawMesh(const Mesh& m, const Shader& sh, const glm::mat4& mvp, const Texture& texture);

private:
};