#include "../include/camera.h"

Camera::Camera(glm::vec3 positionIn = glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
	float yawIn = YAW, float pitchIn = PITCH)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = positionIn;
	worldUp = up;
	yawIn = yawIn;
	pitchIn = pitchIn;
	updateCameraVectors();
}

Camera::Camera(float posX,
	float posY,
	float posZ,
	float upX,
	float upY,
	float upZ,
	float yawIn,
	float pitchIn)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	yaw = yawIn;
	pitch = pitchIn;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboardEvent(CAMERA_MOVEMENT direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		position += front * velocity;
	if (direction == BACKWARD)
		position -= front * velocity;
	if (direction == LEFT)
		position -= right* velocity;
	if (direction == RIGHT)
		position += right * velocity;
}

void Camera::ProcessMouseEvent(float xOffset, float yOffset, GLboolean constrainPitch = true)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
	zoom -= (float)yOffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw) * cos(glm::radians(pitch)));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}