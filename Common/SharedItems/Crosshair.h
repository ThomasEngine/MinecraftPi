#pragma once
#include "Mesh.h"
#include <memory>

class Renderer;
class Shader;
class Texture;
class Crosshair
{
public:
	Crosshair(Renderer& ren);
	~Crosshair();

	void Update(int WindowWidth, int WindowHeight);
	void Render(Renderer& ren, Texture& tex);

private:
	Mesh m_CorsairMesh;
	std::unique_ptr<Shader> m_Shader;
	int m_WindowWidth;
	int m_WindowHeight;
};

