#pragma once
#include <glm/glm.hpp>
#include <string>

#define MAXIMUM_BONES 4 

struct Vertex {
	glm::vec3 position;		// 3D (local) coordinates of the vertex
	glm::vec3 normal;		// NOT USED YET ~~~ 3D (local) normal of the vertex, in transformed space
	glm::vec2 texCoords;	// NOT USED YET ~~~ 2D coordinates in the texture. Ranges from 0 to 1. (0, 0) is the bottom left, (1, 1) is the top right)
	glm::vec3 tangent;		// NOT USED YET ~~~ 
	glm::vec3 biTangent;	// NOT USED YET ~~~ 
	int boneIDs[MAXIMUM_BONES];		// NOT USED YET ~~~ IDs of the four bones that influence this vertex
	float weights[MAXIMUM_BONES];		// NOT USED YET ~~~ The weights of the four bones that influence this vertex
};

struct BoneInfo
{
	int id;
	glm::mat4 offsetMatrix;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};