#pragma once
#include "../mc.h"
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position)
		: m_Position(position),
		m_Front(0.0f, 0.0f, -1.0f),
		m_WorldUp(0.0f, 1.0f, 0.0f),
		m_Yaw(-90.0f),
		m_Pitch(0.0f)
	{
		UpdateVectors();
		//m_Right = glm::normalize(glm::cross(m_Front, m_Up));
	}

	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void SetPosition(const glm::vec3& position)
	{
		m_Position = position;
	}

	glm::vec3 GetPosition() const
	{
		return m_Position;
	}

	void MoveForward(float amount)
	{
		m_Position += m_Front * amount;
	}

	void MoveRight(float amount)
	{
		m_Position += m_Right * amount;
	}

	void MovePosition(const glm::vec3& offset)
	{
		m_Position += offset;
	}

	void AddYaw(float amount)
	{
		m_Yaw += amount;
		UpdateVectors();
	}

	void AddPitch(float amount)
	{
		m_Pitch += amount;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		UpdateVectors();
	}
private:
	void UpdateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Yaw;
	float m_Pitch;
};