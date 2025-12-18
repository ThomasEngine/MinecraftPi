#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include "Mesh.h"
#include <imgui/imgui_impl_opengl3.h>

#ifdef WINDOWS_BUILD
//include glad and glfw for Windows build
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gui.h>
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



struct BlockInstanceData;
class Chunk;
class Shader;
class Texture;
class Renderer {
public:
	// Constructor / Destructor
    Renderer();
    ~Renderer();

	// Initialization and shutdown
    bool init();
    void shutdown();

	// Frame management
    void beginFrame();
    void endFrame();

	// Mesh management
    void uploadMesh(Mesh& mesh);
    void destroyMesh(Mesh& m);

    // Settings
	void drawBothFaces(bool enable);

	// Drawing
    void drawMesh(const Mesh& m, const Shader& sh, const glm::mat4& mvp, const Texture& texture, GLenum primitiveType = GL_TRIANGLES);
};