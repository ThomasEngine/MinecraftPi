#include "Mesh.h"

Mesh::Mesh(const std::vector<FaceVertex>& verts, const std::vector<unsigned int>& inds)
{
	this->vertices = verts;
	this->indices = inds;
	this->indexCount = static_cast<GLsizei>(inds.size());
}
