#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// Camera Translation Directions
enum Movement_Direction {
	FWD,
	AFT,
	LEFT,
	RIGHT
};

// Constraints and Defaults
const float PITCH_CONSTRAINT = 90.0f;
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const float Z_NEAR = 0.1f;
const float Z_FAR = 100.0f;
const glm::vec3 FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

class Camera {
public:
	// Position Attributes
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 world_up;

	// Rotation Attributes
	float yaw;
	float pitch;

	// Settings
	float movement_speed;
	float look_sensitivity;
	float fov;

	// Customizable Constructor
	Camera(glm::vec3 start_position, float start_yaw, float start_pitch, glm::vec3 start_up);
	// Simple Constructor
	Camera(glm::vec3 start_position);

	~Camera();

	// Get View Matrix
	glm::mat4 GetCurrentViewMatrix();

	// Get Projection Matrix
	glm::mat4 GetCurrentProjectionMatrix(float screen_width, float screen_height);
	
	//std::array<glm::vec3, 4> LightSource(float distance, const glm::vec3& center);
	//glm::vec3 GetCameraPosition(glm::mat4 viewMatrix);

	// Move Camera from Keyboard Input
	void MoveCamera(Movement_Direction direction, float deltaTime);

	// Rotate Camera from Mouse Movement
	void RotateCamera(float x_offset, float y_offset);

private:
	// Update Camera Vectors after Rotation (also after constructing)
	void UpdateCamera();
};