#pragma once
#include <glm/glm.hpp>

class Camera{
	public:
		glm::mat4x4 GetCurrentProjectionMatrix();
};