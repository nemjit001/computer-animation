#pragma once
#include "Vertex.hpp"

#include <vector>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh 
{
public:
	// Delete copy and assignment operators
	//Mesh(Mesh const&) = delete;
	Mesh& operator=(Mesh const&) = delete;

	// constructors
	Mesh();
	Mesh(std::string const& filename);
	~Mesh();
	void Render();

private:
	Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices);

	void parse(const aiNode* node, const aiScene* scene);
	void parse(const aiMesh* mesh, const aiScene* scene);

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	// std::vector<Bone> m_bones;
	std::vector<std::unique_ptr<Mesh>> m_subMeshes;

	unsigned int m_VertexBufferObject;
	unsigned int m_IndexBufferObject;
	unsigned int m_VertexArrayObject;
};