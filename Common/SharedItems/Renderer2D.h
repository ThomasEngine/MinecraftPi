#pragma once
#include <Renderer.h>
#include <string>
// This file is for UI rendering in 2D
// For 3D rendering use the 3D Renderer
// UI is rendered from a UI atlas
// This file will contain Sprite struct. The Renderer2D will use thise struct to convert to quads
// Then UI widgets will use the Renderer2D to render themselves
// The Renderer2D will batch render the quads for performance

struct Sprite {
	int u, v; // UV coordinates in the atlas (Top-Left)
	int width, height; // Size of the sprite
};

struct UIVertex {
	float x, y; // Position
	float u, v; // Texture coordinates
	uint32_t color; // Color in RGBA format
};

constexpr size_t maxVertices = 1000;

class Renderer2D
{
public:
	Renderer2D();
	~Renderer2D();
	bool init(Texture* guiTex, Shader* sh);
	void shutdown();
	void beginFrame(int screenW, int screenH);
	void endFrame();
	void drawSprite(const Sprite& sprite, float x, float y, float width, float height, uint32_t color, bool flipY = false);
	void drawQuad(float x, float y, float width, float height, float u, float v, float uWidth, float vHeight, uint32_t color);
	void drawText(const std::string& text, float x, float y, float scale);

protected:
	void flush();

private:
	// Internal data for batching and rendering
	GLuint vao = 0;
	GLuint vbo = 0;

	Texture* textureAtlas = nullptr;
	Shader* shader = nullptr;
	glm::mat4 proj;

	std::vector<UIVertex> vertices;
		
};

