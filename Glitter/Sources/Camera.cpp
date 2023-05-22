#pragma once

#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 start_position, float start_yaw, float start_pitch, glm::vec3 start_up) : front(FRONT), movement_speed(SPEED), look_sensitivity(SENSITIVITY), fov(FOV)
{
	position = start_position;
	yaw = start_yaw;
	pitch = start_pitch;
	world_up = up;
	UpdateCamera();
}

Camera::Camera(glm::vec3 start_position) : front(FRONT), movement_speed(SPEED), look_sensitivity(SENSITIVITY), fov(FOV)
{
	position = start_position;
	yaw = YAW;
	pitch = PITCH;
	world_up = UP;
	enabled = true;

	UpdateCamera();
}

Camera::~Camera()
{
	// TODO: Shoot Cameraman!
}

void Camera::MoveCamera(Movement_Direction direction, float deltaTime)
{
	// If camera is disabled ignore
	if (!enabled)
		return;

	float smooth_speed = movement_speed * deltaTime;

	if (direction == FWD)
		position += front * smooth_speed;
	else if (direction == AFT)
		position -= front * smooth_speed;
	else if (direction == LEFT)
		position -= right * smooth_speed;
	else if (direction == RIGHT)
		position += right * smooth_speed;
}

void Camera::RotateCamera(float x_offset, float y_offset)
{
	// If camera is disabled ignore
	if (!enabled)
		return;

	// Update Pitch and Yaw
	yaw += x_offset * look_sensitivity;
	pitch += y_offset * look_sensitivity;

	// Constraint Pitch
	if (pitch > PITCH_CONSTRAINT)
		pitch = PITCH_CONSTRAINT;
	else if (pitch < -PITCH_CONSTRAINT)
		pitch = -PITCH_CONSTRAINT;

	// Update Camera Vectors after Rotation
	UpdateCamera();
}

glm::mat4 Camera::GetCurrentViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetCurrentProjectionMatrix(float screen_width, float screen_height)
{
	return glm::perspective(glm::radians(fov), screen_width / screen_height, Z_NEAR, Z_FAR);
}

void Camera::UpdateCamera()
{
	// Update Front Vector
	glm::vec3 new_front = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	front = glm::normalize(new_front);

	// Update Right and Up Vectors
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}