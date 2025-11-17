#include <iostream>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "Chunk.h"
#include "BlockRegistery.h"

// GLERROR
void GLClearError()
{
    while (glGetError()); // while it is not 0;
}

void GLCheckError()
{
    while (GLenum error = glGetError()) // while error is not 0
    {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): "
            << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

// Renderer
Renderer::Renderer() {}
Renderer::~Renderer() { shutdown(); }

bool Renderer::init() {

    // GL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    cubeMesh = createCubeMesh();

    return true;
}

void Renderer::shutdown() {
}

void Renderer::beginFrame() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

}

void Renderer::endFrame() {
}

Mesh Renderer::uploadMesh(const std::vector<float>& vertexData, const std::vector<unsigned int>& indices) {
    Mesh m;
    m.indexCount = static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return m;
}

Mesh Renderer::uploadMesh(const std::vector<FaceVertex>& vertexData, const std::vector<unsigned int>& indices) {
    Mesh m;
    m.indexCount = static_cast<GLsizei>(indices.size());
    GLCall(glGenVertexArrays(1, &m.vao));
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(FaceVertex), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute 0: position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, pos));
    // Attribute 1: texcoord (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, tex));
    // Attribute 2: cellX
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, cellX));
    // Attribute 3: cellY
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, cellY));


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return m;
}


void Renderer::destroyMesh(Mesh& m) {
    if (m.ibo) { glDeleteBuffers(1, &m.ibo); m.ibo = 0; }
    if (m.vbo) { glDeleteBuffers(1, &m.vbo); m.vbo = 0; }
    if (m.vao) { glDeleteVertexArrays(1, &m.vao); m.vao = 0; }
    m.indexCount = 0;
}

Mesh Renderer::createCubeMesh() {
    // 24 vertices (4 per face), 36 indices (6 per face)
    const float verts[] = {
        //  X      Y      Z      U     V
        // Front (+Z)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        // Back (-Z)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        // Left (-X)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        // Right (+X)
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         // Top (+Y)
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         // Bottom (-Y)
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    };

    const uint32_t inds[] = {
        // Front (+Z)
        0, 1, 2,  0, 2, 3,
        // Back (-Z)
        4, 6, 5,  4, 7, 6, 
        // Left (-X)
        8, 9,10,  8,10,11,
        // Right (+X)
        12,13,14, 12,14,15,
        // Top (+Y)
        16,17,18, 16,18,19,
        // Bottom (-Y)
        20,21,22, 20,22,23
    };



    std::vector<float> v(std::begin(verts), std::end(verts));
    std::vector<uint32_t> i(std::begin(inds), std::end(inds));
    return uploadMesh(v, i);
}

// ~1500 fps and 180 mb memory
void Renderer::drawMesh(const Mesh& m, const Shader& sh, const glm::mat4& mvp, const Texture& texture) {
    GLuint program = sh.GetID();
    GLuint tex = texture.GetID();
    if (m.vao == 0 || m.indexCount == 0 || program == 0)
        return;
    glUseProgram(program);
    GLint loc = glGetUniformLocation(program, "u_MVP");
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

    if (tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        GLint tloc = glGetUniformLocation(program, "u_Tex");
        if (tloc >= 0) glUniform1i(tloc, 0);
    }

    glBindVertexArray(m.vao);
    glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
