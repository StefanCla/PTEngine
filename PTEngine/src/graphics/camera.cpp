#include "precomp.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "graphics/camera.hpp"

namespace PT
{

	Camera::Camera(glm::vec3 StartPosition = glm::vec3(0.0f))
		: m_CameraPosition(StartPosition)
	{
		const glm::vec3 ReverseCameraDir = glm::normalize(m_CameraPosition - m_Target);

		m_CameraRightDir = glm::normalize(glm::cross(m_WorldUp, ReverseCameraDir));
		m_CameraUpDir = glm::cross(ReverseCameraDir, m_CameraRightDir);
	}

	void Camera::UpdateCameraPosition(unsigned char KeyState, float DeltaTime)
	{
		if(KeyState & KEY_W)
			m_CameraPosition += (m_CameraSpeed * DeltaTime) * m_CameraFrontDir;

		if(KeyState & KEY_A)
			m_CameraPosition -= glm::normalize(glm::cross(m_CameraFrontDir, m_CameraUpDir)) * (m_CameraSpeed * DeltaTime);

		if(KeyState & KEY_S)
			m_CameraPosition -= (m_CameraSpeed * DeltaTime) * m_CameraFrontDir;

		if(KeyState & KEY_D)
			m_CameraPosition += glm::normalize(glm::cross(m_CameraFrontDir, m_CameraUpDir)) * (m_CameraSpeed * DeltaTime);
	}

	void Camera::UpdateCameraDirection(const SDL_MouseMotionEvent& MotionEvent, float DeltaTime)
	{
		float OffsetX = static_cast<float>(MotionEvent.xrel) * m_MouseSensitivity;
		float OffsetY = (static_cast<float>(MotionEvent.yrel) * m_MouseSensitivity) * -1.0f;

		m_Yaw += OffsetX;
		m_Pitch += OffsetY;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		glm::vec3 CameraDir;
		CameraDir.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		CameraDir.y = sin(glm::radians(m_Pitch));
		CameraDir.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_CameraFrontDir = glm::normalize(CameraDir);
	}

	const glm::mat4& Camera::CalculateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_CameraPosition, (m_CameraPosition + m_CameraFrontDir), m_CameraUpDir);
		return m_ViewMatrix;
	}

};