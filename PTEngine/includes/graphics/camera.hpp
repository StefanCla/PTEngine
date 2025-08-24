#pragma once
#include <glm/glm.hpp>
#include <SDL3/SDL_events.h>

namespace PT
{

	class Camera
	{
	public:
		Camera(glm::vec3 StartPosition);

		void UpdateCameraPosition(unsigned char KeyState, float DeltaTime);
		void UpdateCameraDirection(const SDL_MouseMotionEvent& MotionEvent, float DeltaTime);
		const glm::mat4& CalculateViewMatrix();

		const glm::vec3& GetCameraPos() const { return m_CameraPosition; }

	private:
		const glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		const glm::vec3 m_Target = glm::vec3(0.0f);

		glm::vec3 m_CameraPosition = glm::vec3(0.0f);

		glm::vec3 m_CameraFrontDir = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_CameraRightDir = glm::vec3(0.0f);
		glm::vec3 m_CameraUpDir = glm::vec3(0.0f);

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_CameraSpeed = 10.0f;
		float m_MouseSensitivity = 0.1f;

	};

};