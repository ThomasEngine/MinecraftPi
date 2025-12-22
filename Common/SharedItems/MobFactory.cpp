#include "MobFactory.h"
#include "Mobs.h"
#include "Texture.h"

extern const FaceVertex faceVertices[6][4];

namespace {
	void AddCuboid(
		Mesh& mesh,
		const glm::vec3& min, const glm::vec3& max,
		const glm::vec2 cell[6]
	) {
		unsigned int indexOffset = mesh.vertices.size();
        for (int face = 0; face < 6; ++face) {
            for (int v = 0; v < 4; ++v) {
                glm::vec3 p = faceVertices[face][v].pos;
				glm::vec3 pos = glm::mix(min, max, p); 
                glm::vec2 tex = faceVertices[face][v].tex;
				mesh.vertices.push_back({ pos, tex, cell[face].x, cell[face].y, 1 }); 
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
	void LoadPigModel(Renderer& ren, SharedModelData& sheepModel) {
		
		// Load textures
		Texture* texture = new Texture("Common/SharedItems/Assets/Mobs/temperate_pig.png");
		sheepModel.texture = texture;

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

		// Texture atlas cells
		// Head
		glm::vec2 cellHead[6] = {
			glm::vec2(1, 7), // Back
			glm::vec2(1, 6), // Front
			glm::vec2(2, 6), // Bottom
			glm::vec2(3, 6), // Top
			glm::vec2(0, 6), // Left
			glm::vec2(2, 6)  // Right
		};
		
		// Body
		glm::vec2 cellBody[6] = {
			glm::vec2(4, 6),  // Back
			glm::vec2(3, 6), // Front
			glm::vec2(6, 5), // Bottom
			glm::vec2(4, 5), // Top
			glm::vec2(3, 5), // Left
			glm::vec2(3, 5), // Right
		};

		// Legs
		glm::vec2 cellLeg[6] = {
			glm::vec2(1, 7), // Back
			glm::vec2(3, 7), // Front
			glm::vec2(1, 7), // Bottom
			glm::vec2(1, 7), // Top
			glm::vec2(1, 7), // Left
			glm::vec2(1, 7)  // Right
		};


		AddCuboid(sheepModel.headMesh, headMin, headMax, cellHead);
		AddCuboid(sheepModel.bodyMesh, bodyMin, bodyMax, cellBody);
		AddCuboid(sheepModel.legMesh[Legs::FrontLeft], legFLMin, legFLMax, cellLeg);
		AddCuboid(sheepModel.legMesh[Legs::FrontRight], legFRMin, legFRMax, cellLeg);
		AddCuboid(sheepModel.legMesh[Legs::BackLeft], legBLMin, legBLMax, cellLeg);
		AddCuboid(sheepModel.legMesh[Legs::BackRight], legBRMin, legBRMax, cellLeg);

		ren.uploadMesh(sheepModel.bodyMesh);
		ren.uploadMesh(sheepModel.headMesh);
		ren.uploadMesh(sheepModel.legMesh[Legs::FrontLeft]);
		ren.uploadMesh(sheepModel.legMesh[Legs::FrontRight]);
		ren.uploadMesh(sheepModel.legMesh[Legs::BackLeft]);
		ren.uploadMesh(sheepModel.legMesh[Legs::BackRight]);

		// Leg origin
		sheepModel.legTopPosition[Legs::FrontLeft] = glm::vec3(-0.25f, 0.4f, 0.5f);
		sheepModel.legTopPosition[Legs::FrontRight] = glm::vec3(0.5f, 0.4f, 0.5f);
		sheepModel.legTopPosition[Legs::BackLeft] = glm::vec3(-0.25f, 0.4f, -0.5f);
		sheepModel.legTopPosition[Legs::BackRight] = glm::vec3(0.25f, 0.4f, -0.5f);

		// Init lighting

		sheepModel.hitbox = glm::vec3(0.9f, 0.f, 1.3f);
	}

	Mesh LoadVillagerModel(Renderer& ren) {
		Mesh mesh;
		// Add body
		//AddCuboid(mesh, glm::vec3(-0.4f, 0.0f, -0.3f), glm::vec3(0.4f, 0.9f, 0.3f), 0.0f, 0.0f);
		//// Add head
		//AddCuboid(mesh, glm::vec3(-0.25f, 0.9f, -0.25f), glm::vec3(0.25f, 1.3f, 0.25f), 1.0f, 0.0f);
		//// Add legs
		//AddCuboid(mesh, glm::vec3(-0.2f, 0.0f, -0.15f), glm::vec3(-0.05f, 0.5f, 0.15f), 2.0f, 0.0f);
		//AddCuboid(mesh, glm::vec3(0.05f, 0.0f, -0.15f), glm::vec3(0.2f, 0.5f, 0.15f), 2.0f, 0.0f);

		ren.uploadMesh(mesh);

		return mesh;
	}
}


void InitializeMobPrototypes(MobPrototypeRegistry& registry, Renderer& ren)
{
	SharedModelData* sheepModel = new SharedModelData();
	LoadPigModel(ren, *sheepModel);
	registry.registerPrototype("Sheep", new FourlegMob(sheepModel));


	//SharedModelData* villagerModel = new SharedModelData();
	//villagerModel->mesh = LoadVillagerModel(ren);
	//registry.registerPrototype("Villager", new Villager(villagerModel));
}

MobFactory::~MobFactory()
{
	// TODO:: CLEANUP POINTERS	
}
