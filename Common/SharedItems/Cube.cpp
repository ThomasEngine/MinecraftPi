#include "Cube.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"


Cube::Cube(const Mesh* mesh, const Texture* tex, const Shader* shader)
	: 
	m_Mesh(mesh),
	m_Texture(tex),
	m_Shader(shader)
{
}

Cube::~Cube()
{
}

void Cube::Draw(Renderer& ren, const glm::mat4& viewProj) const
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Position);
    model = glm::rotate(model, m_Rotation.y, glm::vec3(0, 1, 0));
    model = glm::scale(model, m_Scale);
    glm::mat4 mvp = viewProj * model;
    ren.drawMesh(*m_Mesh, *m_Shader, mvp, *m_Texture);
}
