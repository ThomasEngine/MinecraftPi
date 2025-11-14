#pragma once
#include <glm/glm.hpp>


struct Mesh;
class Texture;
class Renderer;
class Shader;
class Cube {
public:
	Cube(const Mesh* mesh, const Texture* tex, const Shader* shader);
	~Cube();

	void SetPosition(const glm::vec3& pos) { m_Position = pos; }
	void SetRotation(const glm::vec3& rot) { m_Rotation = rot; }
	void SetScale(const glm::vec3& sc) { m_Scale = sc; }

	void Draw(Renderer& ren, const glm::mat4& viewProj) const;
private:
	const Mesh* m_Mesh;
	const Texture* m_Texture;
	const Shader* m_Shader;
	glm::vec3 m_Position{ 0.0f };
	glm::vec3 m_Rotation{ 0.0f };
	glm::vec3 m_Scale{ 1.0f };
};