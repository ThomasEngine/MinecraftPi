#include "Crosshair.h"
#include <glm/glm.hpp>
#include "Renderer.h"
#include "Shader.h"
#include <Texture.h>

Crosshair::Crosshair(Renderer& ren)
   : m_WindowWidth(0), m_WindowHeight(0)
{
   m_Shader = std::make_unique<Shader>("Common/SharedItems/Assets/crosshair.shader");

   // Create crosshair mesh
   std::vector<FaceVertex> vertices = {
       { glm::vec3(-0.02f,  0.02f, 0.0f), glm::vec2(0.0f, 1.0f), 0.0f, 0.0f, 1.0f }, 
       { glm::vec3( 0.02f,  0.02f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f, 0.0f, 1.0f }, 
       { glm::vec3( 0.02f, -0.02f, 0.0f), glm::vec2(1.0f, 0.0f), 0.0f, 0.0f, 1.0f }, 
       { glm::vec3(-0.02f, -0.02f, 0.0f), glm::vec2(0.0f, 0.0f), 0.0f, 0.0f, 1.0f }  
   };
   std::vector<unsigned int> indices = {
       0, 1, 2,
       0, 2, 3
   };
   m_CorsairMesh = Mesh(vertices, indices);
   ren.uploadMesh(m_CorsairMesh);
}

Crosshair::~Crosshair()
{
}

void Crosshair::Update(int WindowWidth, int WindowHeight)
{
   m_WindowWidth = WindowWidth;
   m_WindowHeight = WindowHeight;
}

void Crosshair::Render(Renderer& ren, Texture& tex)
{
   m_Shader->Bind();
   ren.drawMesh(m_CorsairMesh, *m_Shader, glm::mat4(1), tex, GL_LINES);
   m_Shader->Unbind();
}
