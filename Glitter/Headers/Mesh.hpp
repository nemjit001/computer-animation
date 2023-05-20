#pragma once
#include "Vertex.hpp"
#include "Face.hpp"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh 
{
public:
	// Delete copy and assignment operators
	Mesh(Mesh const&) = delete;
	Mesh& operator=(Mesh const&) = delete;

	// constructors
	Mesh();
	Mesh(std::string const& filename);
	~Mesh();
	void Render();

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
	// std::vector<Bone> m_bones;

	unsigned int m_VertexBufferObject;
	unsigned int m_VertexArrayObject;
};