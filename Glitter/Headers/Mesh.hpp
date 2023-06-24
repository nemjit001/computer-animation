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

/// <summary>
/// Implements Meshes that were imported using Assimp
/// </summary>
class Mesh
{
public:
	// Delete copy and assignment operators
	Mesh(Mesh const&) = delete;
	Mesh& operator=(Mesh const&) = delete;

	// constructors
	Mesh(std::string const& filename, const Shader& shader, const Shader& skeletonShader);
	~Mesh();
	void Render(glm::mat4, glm::mat4, glm::mat4, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, GLuint, GLuint, GLuint);
	void RenderBones(glm::mat4, glm::mat4, glm::mat4, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, GLuint, GLuint, GLuint);	
	void Animate(int frame);

	/// <summary>
	/// Traverses nodes (aiNode) in tree recursively, to calculate final transformation matrices
	/// </summary>
	/// <param name="frame">: the keyframe to be animated</param>
	/// <param name="node">: the node currently processed</param>
	/// <param name="parent_transform">: the tranformation matrix of the parent of this node</param>
	void TraverseNode(const int frame, const aiNode* node, const glm::mat4& parent_transform);

	Shader getShader();
	int GetAnimationFrameNum();												// Temp!
	bool HasAnimations();

private:
	Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices, std::vector<Texture> const& textures, const Shader shader, const Shader skeletonShader);

	void Parse(const aiNode* node, const aiScene* scene);
	void Parse(const aiMesh* mesh, const aiScene* scene);

	/// <summary>
	/// Parses all animations in the imported mesh
	/// </summary>
	/// <param name="scene">: the scene of this mesh</param>
	void ParseAnimations(const aiScene* scene);

	/// <summary>
	/// Sets bone information to a vertex it's affecting
	/// </summary>
	/// <param name="vertex">: the vertex</param>
	/// <param name="boneId">: the ID of the bone</param>
	/// <param name="weight">: the weight of the bone</param>
	void SetVertexBoneData(Vertex& vertex, int boneId, float weight);

	/// <summary>
	/// Extracts bone information and stores it in vertices
	/// </summary>
	/// <param name="vertices">: the mesh vertices</param>
	/// <param name="mesh">: the mesh</param>
	/// <param name="scene">: the scene of the mesh</param>
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, const aiMesh* mesh, const aiScene* scene);

	/// <summary>
	/// Prepares the VAO and VBO for the skeleton rendering
	/// </summary>
	void PrepareSkeleton();

	/// <summary>
	/// Loads textures bases on type
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="type"></param>
	/// <param name="typeName"></param>
	/// <returns></returns>
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	/// <summary>
	/// Loads textures from file
	/// </summary>
	/// <param name="path"></param>
	/// <param name="directory"></param>
	/// <param name="gamma"></param>
	/// <returns></returns>
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	/// <summary>
	/// Conversion from aiMatrix4x4 to glm::mat4
	/// </summary>
	/// <param name="from">: source matrix</param>
	/// <returns></returns>
	inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

	/// <summary>
	/// Conversion from aiVector3D to glm::vec3
	/// </summary>
	/// <param name="src">: source vector</param>
	/// <returns></returns>
	inline glm::vec3 ConvertVector3DToGLMFormat(const aiVector3D& src);

	/// <summary>
	/// Conversion from aiQuaternion to glm::quat
	/// </summary>
	/// <param name="src">: source quaternion</param>
	/// <returns></returns>
	inline glm::quat ConvertQuaternionToGLMFormat(const aiQuaternion& src);

	std::vector<Vertex> m_vertices;												// Vertices of Mesh (Vertex struct)
	std::vector<unsigned int> m_indices;										// Indices for rendering
	unsigned int m_boneVertexCount;												// Number of vertices for rendering the bone (part of the skeleton)
	std::vector<Texture> m_textures;											// Textures associated with this mesh
	std::map<std::string, int> bone_map;										// Map connects node - bone names to indices in m_bones vector
	std::vector<BoneInfo> m_bones;												// Is indexed by the indices in bone_map
	std::vector<AnimationClip> m_animations;									// Animations associated with this mesh
	std::string dir;															// Mesh directory
	Assimp::Importer importer;													// Assimp Importer for the scene (MUST LIVE!!!)
	const aiScene* scene;														// Points to scene of the mesh. Needed to preserve node tree for bone transformation calculations
	int m_boneCounter = 0;														// Number of bones in mesh rig
	glm::mat4 inverse_transform;												// Inverse transform matrix for mesh to scene. Possibly only useful if more submeshes are used
	Shader shader;																// Shader used for rendering this mesh (Shader class)
	Shader skeletonShader;														// Shader used for rendering the skeleton (Shader class)
	std::vector<std::unique_ptr<Mesh>> m_subMeshes;								// Who knows at this point

	// Buffer - Array Objects
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_VAO;

	unsigned int m_skeletonVBO;
	unsigned int m_skeletonVAO;
};