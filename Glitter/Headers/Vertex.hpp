#pragma once
#include <glm/glm.hpp>
#include <string>

#define MAXIMUM_BONES 4 

/// <summary>
/// Struct containing Vertex information
/// </summary>
struct Vertex {
	glm::vec3 position;							// 3D (local) coordinates of the vertex
	glm::vec3 normal;							// 3D (local) normal of the vertex, in transformed space
	glm::vec2 texCoords;						// 2D coordinates in the texture. Ranges from 0 to 1. (0, 0) is the bottom left, (1, 1) is the top right)
	glm::vec3 tangent;							// NOT USED YET ~~~ 
	glm::vec3 biTangent;						// NOT USED YET ~~~
	unsigned int bone_num = 0;					// Number of bones affecting the vertex. Used to add bone IDs and weights properly
	int boneIDs[MAXIMUM_BONES] = { 0 };			// Initialized to zeros
	float weights[MAXIMUM_BONES] = { 0.0f };	// Initialized to zeros
};

/// <summary>
/// Struct containing Bone information
/// </summary>
struct BoneInfo
{
	int id;										// Bone ID index
	glm::mat4 offsetMatrix;						// Offset matrix for bone space
	glm::mat4 bone_transform = glm::mat4(0.0f);	// Final bone tranformation matrix
	glm::mat4x2 dual_quat = glm::mat4x2(0.0f);	// Final bone dual quaternion
};

/// <summary>
/// Struct containing Texture information
/// </summary>
struct Texture
{
	unsigned int id;							// Texture ID
	std::string type;							// Type of texture (diffuse, normal, specular, height)
	std::string path;							// Expected path of texture
};