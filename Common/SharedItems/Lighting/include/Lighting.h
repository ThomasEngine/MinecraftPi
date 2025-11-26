#pragma once
#include <queue>
#include <glm/glm.hpp>


class World;
class Lighting
{
public:
	Lighting();
	~Lighting();

	void enqueueLightSource(const glm::vec3& pos, int strength);
	void propagateLight();



private:
	std::queue<glm::vec3> floodQueue;
	World* world;



};