#include "Renderer2D.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Texture.h"

Renderer2D::Renderer2D()
{
}

Renderer2D::~Renderer2D()
{
}

bool Renderer2D::init(Texture* guiTex, Shader* sh)
{
	textureAtlas = guiTex;
	shader = sh;
	// Create VAO and VBO
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(UIVertex), nullptr, GL_DYNAMIC_DRAW)); // Dynamic draw is used for frequently updated data

	// Define vertex attributes
	GLCall(glEnableVertexAttribArray(0)); // Position
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, x)));
	GLCall(glEnableVertexAttribArray(1)); // Texture coordinates
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, u)));
	GLCall(glEnableVertexAttribArray(2)); // Color
	GLCall(glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(UIVertex), (const void*)offsetof(UIVertex, color)));

	GLCall(glBindVertexArray(0));
	return true;
}

void Renderer2D::shutdown()
{
	if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
	if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
}

void Renderer2D::beginFrame(int screenW, int screenH)
{
	vertices.clear();

	// Set up orthographic projection
	proj = glm::ortho(0.0f, static_cast<float>(screenW), static_cast<float>(screenH), 0.0f, -1.0f, 1.0f);


	GLCall(glDisable(GL_DEPTH_TEST)); // Disable depth testing for 2D rendering
	GLCall(glDisable(GL_CULL_FACE)); // disable face culling

	GLCall(glUseProgram(shader->GetID()));
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(proj)));

	
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureAtlas->GetID()));
	GLCall(glUniform1i(glGetUniformLocation(shader->GetID(), "u_TextureAtlas"), 0));
}

void Renderer2D::endFrame()
{
	flush();
	GLCall(glEnable(GL_DEPTH_TEST)); // Re-enable depth testing
	GLCall(glEnable(GL_CULL_FACE)); // Re-enable face culling
	glUseProgram(0);
}

void Renderer2D::drawSprite(const Sprite& sprite, float x, float y, float width, float height, uint32_t color, bool FlipY)
{
	// Similair to drawQuad but using sprite UVs
	if (vertices.size() + 6 > maxVertices) {
		flush(); // Flush if exceeding max vertices
	}

	float atlasWidth = float(textureAtlas->GetWidth());
	float atlasHeight = float(textureAtlas->GetHeight());

	float u0 = float(sprite.u) / atlasWidth;
	float v0 = float(sprite.v) / atlasHeight;
	float u1 = float(sprite.u + sprite.width) / atlasWidth;
	float v1 = float(sprite.v + sprite.height) / atlasHeight;

	if (FlipY) {
		std::swap(v0, v1);
	}

	// Define the 6 vertices for the quad (2 triangles)
	vertices.push_back({ x, y, u0, v0, color });
	vertices.push_back({ x + width, y, u1, v0, color });
	vertices.push_back({ x + width, y + height, u1, v1, color });
	vertices.push_back({ x, y, u0, v0, color });
	vertices.push_back({ x + width, y + height, u1, v1, color });
	vertices.push_back({ x, y + height, u0, v1, color });

}

void Renderer2D::drawQuad(float x, float y, float width, float height, float u, float v, float uWidth, float vHeight, uint32_t color)
{
	if (vertices.size() + 6 > maxVertices) {
		flush(); // Flush if exceeding max vertices
	}
	
	// Define the 6 vertices for the quad (2 triangles)
	vertices.push_back({ x, y, u, v, color });
	vertices.push_back({ x + width, y, u + uWidth, v, color });
	vertices.push_back({ x + width, y + height, u + uWidth, v + vHeight, color });
	vertices.push_back({ x, y, u, v, color });
	vertices.push_back({ x + width, y + height, u + uWidth, v + vHeight, color });
	vertices.push_back({ x, y + height, u, v + vHeight, color });
}

void Renderer2D::drawText(const std::string& text, float x, float y, float scale)
{
	// TODO
}

void Renderer2D::flush() // Send batched vertices to GPU and draw
{
	if (vertices.empty()) return;
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(UIVertex), vertices.data()));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size())));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	vertices.clear();
}


