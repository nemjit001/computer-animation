#pragma once
#include "Vertex.hpp"
#include "Shader.hpp"
#include "AnimationClip.hpp"

#include <vector>
#include <memory>
#include <map>
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
	Mesh(std::string const& filename, const Shader& shader);
	~Mesh();
	void Render(glm::mat4, glm::mat4, glm::mat4, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float);
	void Animate(int frame);
	void TraverseNode(const int frame, const aiNode* node, const glm::mat4& parent_transform);
	Shader getShader();
	int GetAnimationFrameNum();												// Temp!
	bool HasAnimations();

private:
	Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices, std::vector<Texture> const& textures, const Shader shader);

	void Parse(const aiNode* node, const aiScene* scene);
	void Parse(const aiMesh* mesh, const aiScene* scene);
	void ParseAnimations(const aiScene* scene);
	void SetBoneToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneId, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, const aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
	inline glm::mat4 Mesh::ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	inline glm::vec3 Mesh::ConvertVector3DToGLMFormat(const aiVector3D& src);
	inline glm::quat Mesh::ConvertQuaternionToGLMFormat(const aiQuaternion& src);

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	std::map<std::string, int> bone_map;										// Map connects node - bone names to indices in m_bones vector
	std::vector<BoneInfo> m_bones;												// Is indexed by the indices in bone_map
	std::vector<AnimationClip> m_animations;
	std::string dir;
	Assimp::Importer importer;
	const aiScene* scene;
	int m_boneCounter = 0;
	glm::mat4 inverse_transform;
	Shader shader;
	std::vector<std::unique_ptr<Mesh>> m_subMeshes;

	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_VAO;
};