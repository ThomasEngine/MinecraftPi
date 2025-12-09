#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <Mesh.h>

class Shader;
class Renderer;
class Texture;
class OnBlock
{
public:
	OnBlock(Renderer& ren);
	~OnBlock();
	void StartBreaking(const glm::ivec3& blockPos, const uint8_t& breakStrength);
	void Update(float deltaTime, const glm::ivec3& blockPos);
	void Render(Renderer& ren, glm::mat4 viewProj, Texture& tex);

private:
	std::unique_ptr<Shader> m_Shader;
	glm::ivec3 m_blockPos;
	bool m_isBreaking = false;
	float m_breakProgress = 0.0f;
	uint8_t m_breakStrength = 0;
	Mesh m_cubeMesh;
};

