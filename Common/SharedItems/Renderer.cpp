#include <iostream>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "Chunk.h"
#include "BlockRegistery.h"
#include "Mesh.h"


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

    return true;
}

void Renderer::shutdown() {
}

void Renderer::beginFrame() {

}

void Renderer::endFrame() {
}

void Renderer::uploadMesh(Mesh& m)
{
    m.indexCount = static_cast<GLsizei>(m.indices.size());
    GLCall(glGenVertexArrays(1, &m.vao));
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(FaceVertex), m.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(unsigned int), m.indices.data(), GL_STATIC_DRAW);

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

    // Attribute 4: light
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, light));

	// Attribute 5: ao
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), (const void*)offsetof(FaceVertex, ao));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Renderer::destroyMesh(Mesh& m) {
    if (m.ibo) { glDeleteBuffers(1, &m.ibo); m.ibo = 0; }
    if (m.vbo) { glDeleteBuffers(1, &m.vbo); m.vbo = 0; }
    if (m.vao) { glDeleteVertexArrays(1, &m.vao); m.vao = 0; }
    m.indexCount = 0;
}

void Renderer::drawBothFaces(bool enable)
{
    if (enable)
    {
		glDisable(GL_CULL_FACE);
    }
	else
		glEnable(GL_CULL_FACE);
}

void Renderer::startBatch(const Shader& sh, const glm::mat4& mvp, const Texture& texture)
{
    GLuint program = sh.GetID();
    GLuint tex = texture.GetID();
    if (program == 0) return;
        
    glUseProgram(program);
    GLint loc = glGetUniformLocation(program, "u_MVP");
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

    if (tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        GLint tloc = glGetUniformLocation(program, "u_TextureAtlas");
        if (tloc >= 0) glUniform1i(tloc, 0);
    }
}



void Renderer::endBatch()
{
	glUseProgram(0);
}

void Renderer::drawBatchMesh(const Mesh& m)
{
    if (m.vao == 0 || m.indexCount == 0) return;

	glBindVertexArray(m.vao);
	glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Renderer::drawMesh(const Mesh& m, const Shader& sh, const glm::mat4& mvp, const Texture& texture, GLenum primitiveType) {
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
        GLint tloc = glGetUniformLocation(program, "u_TextureAtlas");
        if (tloc >= 0) glUniform1i(tloc, 0);
    }

    glBindVertexArray(m.vao);
    glDrawElements(primitiveType, m.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
