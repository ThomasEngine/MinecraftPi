#include "OnBlock.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef WINDOWS_BUILD
#include <glad/glad.h>
#include <imgui/imgui_impl_opengl3_loader.h>
#elif defined(Raspberry_BUILD)
#include <GLES3/gl3.h>
#else
#include <GLES2/gl2.h>
#endif

extern const FaceVertex faceVertices[6][4];

constexpr float breakStrengths[10] = {
	0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
	0.6f, 0.7f, 0.8f, 0.9f, 1.0f
};

static const glm::vec3 edgeVerts[8] = {
	{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
	{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
};
static const unsigned int edgeIndices[24] = {
	0,1, 1,2, 2,3, 3,0, // bottom
	4,5, 5,6, 6,7, 7,4, // top
	0,4, 1,5, 2,6, 3,7  // sides
};


OnBlock::OnBlock(Renderer& ren)
    : m_blockPos(0), m_isBreaking(false), m_breakProgress(0.0f)
{

	// Create cube mesh
    float inset = 0.005f;
    std::vector<FaceVertex> outlineVerts;
    std::vector<unsigned int> outlineIndices;
    unsigned int indexOffset = 0;

    for (int face = 0; face < 6; ++face) {
        // Compute face normal
        glm::vec3 v0 = faceVertices[face][0].pos;
        glm::vec3 v1 = faceVertices[face][1].pos;
        glm::vec3 v2 = faceVertices[face][2].pos;
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add 4 inset vertices for this face
        for (int v = 0; v < 4; ++v) {
			glm::vec3 pos = faceVertices[face][v].pos + normal * inset; // against z fighting
            outlineVerts.push_back({ pos, glm::vec2(0.0f), 0.0f, 0.0f, 1.0f });
        }
        outlineIndices.push_back(indexOffset + 0);
        outlineIndices.push_back(indexOffset + 1);
        outlineIndices.push_back(indexOffset + 1);
        outlineIndices.push_back(indexOffset + 2);
        outlineIndices.push_back(indexOffset + 2);
        outlineIndices.push_back(indexOffset + 3);
        outlineIndices.push_back(indexOffset + 3);
        outlineIndices.push_back(indexOffset + 0);
        indexOffset += 4;
    }


	m_OutlineMesh = Mesh(outlineVerts, outlineIndices);
	ren.uploadMesh(m_OutlineMesh);

    // Break mesh


}

OnBlock::~OnBlock() {}

void OnBlock::StartBreaking(const glm::ivec3& blockPos, const uint8_t& breakingStrength)
{
	m_breakStrength = breakingStrength;
    m_blockPos = blockPos;
    m_isBreaking = true;
    m_breakProgress = 0.0f;
}

void OnBlock::Update(float deltaTime, const glm::ivec3& blockPos)
{
	m_blockPos = blockPos;
    if (m_isBreaking) {
        m_breakProgress += deltaTime * breakStrengths[m_breakStrength]; 
        if (m_breakProgress > 1.0f) {
            m_breakProgress = 1.0f;
            m_isBreaking = false; 
        }
    }
}

void OnBlock::Render(Renderer& ren, glm::mat4 viewProj, Texture& tex, Shader& sh)
{
    if (m_blockPos == glm::ivec3(-1)) return;

    sh.Bind();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_blockPos));
	glm::mat4 mvp = viewProj * model;
	sh.SetUniformMat4f("u_MVP", mvp);

	glDepthMask(GL_FALSE); 
	glLineWidth(2.f);
	ren.drawMesh(m_OutlineMesh, sh, mvp, tex, GL_LINES);
	glDepthMask(GL_TRUE);

    sh.Unbind();
}