#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;		// 3D (local) coordinates of the vertex
	glm::vec3 normal;		// NOT USED YET ~~~ 3D (local) normal of the vertex, in transformed space
	glm::vec2 texCoords;	// NOT USED YET ~~~ 2D coordinates in the texture. Ranges from 0 to 1. (0, 0) is the bottom left, (1, 1) is the top right)
	glm::vec3 tangent;		// NOT USED YET ~~~ 
	glm::vec3 biTangent;	// NOT USED YET ~~~ 
	int mBoneIDs[4];		// NOT USED YET ~~~ IDs of the four bones that influence this vertex
	float mWeights[4];		// NOT USED YET ~~~ The weights of the four bones that influence this vertex
};