#include "MobFactory.h"
#include "Mobs.h"

extern const FaceVertex faceVertices[6][4];

namespace {
    void AddCuboid(
        Mesh& mesh,
        glm::vec3 min, glm::vec3 max,
        float cellX, float cellY
    ) {
		unsigned int indexOffset = mesh.vertices.size();
        for (int face = 0; face < 6; ++face) {
            for (int v = 0; v < 4; ++v) {
                glm::vec3 p = faceVertices[face][v].pos;
				glm::vec3 pos = glm::mix(min, max, p); // Linear interpolation
                glm::vec2 tex = faceVertices[face][v].tex;
                mesh.vertices.push_back({ pos, tex, cellX, cellY });
            }
            mesh.indices.push_back(indexOffset + 0);
            mesh.indices.push_back(indexOffset + 1);
            mesh.indices.push_back(indexOffset + 2);
            mesh.indices.push_back(indexOffset + 0);
            mesh.indices.push_back(indexOffset + 2);
            mesh.indices.push_back(indexOffset + 3);
            indexOffset += 4;
        }
    }
	Mesh LoadSheepModel(Renderer& ren) {
		Mesh mesh;
        // Add body
		AddCuboid(mesh, glm::vec3(-0.5f, 0.0f, -1.0f), glm::vec3(0.5f, 0.7f, 1.0f), 0.0f, 0.0f);

		// Add head
		AddCuboid(mesh, glm::vec3(-0.3f, 0.5f, 1.0f), glm::vec3(0.3f, 0.9f, 1.5f), 1.0f, 0.0f);

		// Add legs
		AddCuboid(mesh, glm::vec3(-0.4f, 0.0f, -0.8f), glm::vec3(-0.2f, 0.4f, -0.6f), 2.0f, 0.0f);
		AddCuboid(mesh, glm::vec3(0.2f, 0.0f, -0.8f), glm::vec3(0.4f, 0.4f, -0.6f), 2.0f, 0.0f);
		AddCuboid(mesh, glm::vec3(-0.4f, 0.0f, 0.6f), glm::vec3(-0.2f, 0.4f, 0.8f), 2.0f, 0.0f);
		AddCuboid(mesh, glm::vec3(0.2f, 0.0f, 0.6f), glm::vec3(0.4f, 0.4f, 0.8f), 2.0f, 0.0f);

		ren.uploadMesh(mesh);

		return mesh;
	}

	Mesh LoadVillagerModel(Renderer& ren) {
		Mesh mesh;
		// Add body
		AddCuboid(mesh, glm::vec3(-0.4f, 0.0f, -0.3f), glm::vec3(0.4f, 0.9f, 0.3f), 0.0f, 0.0f);
		// Add head
		AddCuboid(mesh, glm::vec3(-0.25f, 0.9f, -0.25f), glm::vec3(0.25f, 1.3f, 0.25f), 1.0f, 0.0f);
		// Add legs
		AddCuboid(mesh, glm::vec3(-0.2f, 0.0f, -0.15f), glm::vec3(-0.05f, 0.5f, 0.15f), 2.0f, 0.0f);
		AddCuboid(mesh, glm::vec3(0.05f, 0.0f, -0.15f), glm::vec3(0.2f, 0.5f, 0.15f), 2.0f, 0.0f);

		ren.uploadMesh(mesh);

		return mesh;
	}
}


void InitializeMobPrototypes(MobPrototypeRegistry& registry, Renderer& ren)
{
	SharedModelData* sheepModel = new SharedModelData();
	sheepModel->mesh = LoadSheepModel(ren);
	registry.registerPrototype("Sheep", new Sheep(sheepModel));


	SharedModelData* villagerModel = new SharedModelData();
	villagerModel->mesh = LoadVillagerModel(ren);
	registry.registerPrototype("Villager", new Villager(villagerModel));
}
