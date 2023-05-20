#pragma once
#include "Vertex.hpp"
#include "Face.hpp"
#include <vector>

class Mesh 
{
public:
	// Delete copy and assignment operators
	Mesh(Mesh const&) = delete;
	Mesh& operator=(Mesh const&) = delete;

	// constructors
	Mesh();
	Mesh(std::string const & filename);
	Mesh(std::vector<Vertex> verts, std::vector<Face> faces);
	void Render();

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
	// std::vector<Bone> m_bones;

	unsigned int m_VertexBufferObject;
	unsigned int m_VertexArrayObject;
};