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
                mesh.vertices.push_back({ pos, tex, cellX, cellY, 1 });
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
	void LoadSheepModel(Renderer& ren, SharedModelData& sheepModel) {
		
		const int atlasIndex = 2;
		float cellX = float(atlasIndex % 16);
		float cellY = 15 - (atlasIndex / 16);

		// Mins and maxes
		// Body
		glm::vec3 bodyMin(-0.45f, 0.4f, -0.65f);
		glm::vec3 bodyMax(0.45f, 1.1f, 0.65f);

		// Head length: 0.6, 2/3 inside body
		float headLength = 0.6f;
		float headInside = headLength * (2.0f / 3.0f); 
		float headOutside = headLength * (1.0f / 3.0f); 
		float bodyFrontZ = 0.65f;
		float headBackZ = bodyFrontZ - headInside; 
		float headFrontZ = bodyFrontZ + headOutside;

		glm::vec3 headMin(-0.3f, 0.8f, headBackZ); 
		glm::vec3 headMax(0.3f, 1.4f, headFrontZ); 

		// Legs
        glm::vec3 legFLMin(-0.4f, 0.0f, 0.35f); // Front Left
        glm::vec3 legFLMax(-0.1f, 0.4f, 0.65f);

        glm::vec3 legFRMin(0.1f, 0.0f, 0.35f); // Front Right
        glm::vec3 legFRMax(0.4f, 0.4f, 0.65f);

        glm::vec3 legBLMin(-0.4f, 0.0f, -0.65f); // Back Left
        glm::vec3 legBLMax(-0.1f, 0.4f, -0.35f);

        glm::vec3 legBRMin(0.1f, 0.0f, -0.65f); // Back Right
        glm::vec3 legBRMax(0.4f, 0.4f, -0.35f);

		AddCuboid(sheepModel.bodyMesh, bodyMin, bodyMax, cellX, cellY);
		AddCuboid(sheepModel.headMesh, headMin, headMax, cellX, cellY);
		AddCuboid(sheepModel.legMesh[Legs::FL], legFLMin, legFLMax, cellX, cellY);
		AddCuboid(sheepModel.legMesh[Legs::FR], legFRMin, legFRMax, cellX, cellY);
		AddCuboid(sheepModel.legMesh[Legs::BL], legBLMin, legBLMax, cellX, cellY);
		AddCuboid(sheepModel.legMesh[Legs::BR], legBRMin, legBRMax, cellX, cellY);

		ren.uploadMesh(sheepModel.bodyMesh);
		ren.uploadMesh(sheepModel.headMesh);
		ren.uploadMesh(sheepModel.legMesh[Legs::FL]);
		ren.uploadMesh(sheepModel.legMesh[Legs::FR]);
		ren.uploadMesh(sheepModel.legMesh[Legs::BL]);
		ren.uploadMesh(sheepModel.legMesh[Legs::BR]);

		// Leg origin
		sheepModel.legTopPosition[Legs::FL] = glm::vec3(-0.25f, 0.4f, 0.5f);
		sheepModel.legTopPosition[Legs::FR] = glm::vec3(0.5f, 0.4f, 0.5f);
		sheepModel.legTopPosition[Legs::BL] = glm::vec3(-0.25f, 0.4f, -0.5f);
		sheepModel.legTopPosition[Legs::BR] = glm::vec3(0.25f, 0.4f, -0.5f);

		sheepModel.hitbox = glm::vec3(0.9f, 0.f, 1.3f);
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
	LoadSheepModel(ren, *sheepModel);
	registry.registerPrototype("Sheep", new FourlegMob(sheepModel));


	//SharedModelData* villagerModel = new SharedModelData();
	//villagerModel->mesh = LoadVillagerModel(ren);
	//registry.registerPrototype("Villager", new Villager(villagerModel));
}
