#include "Camera.hpp"
#include <glm/glm.hpp>

glm::mat4x4 Camera::GetCurrentProjectionMatrix() {
	// The camera needs to be implemented. For now, we simply return a matrix filled diagonally with 0.5
	// This proves that our matrix is being passed as the triangle's size is halved.
	return glm::mat4(0.5f);
}