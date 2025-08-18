#include "precomp.hpp"

#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"
	
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "graphics/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"

#include <chrono>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

inline void ChangeKeyState(unsigned char& KeyState, int Key)
{
	KeyState = KeyState ^ Key;
}

void Deinitialize(PT::Renderer* Renderer, PT::Shader* Shader, PT::Camera* Camera)
{
	delete Renderer;
	Renderer = nullptr;

	delete Shader;
	Shader = nullptr;

	delete Camera;
	Camera = nullptr;
}

int main()
{
	PT::Renderer* Renderer = new PT::Renderer;
	PT::Shader* Shader = new PT::Shader;

	glm::vec3 CameraStart = glm::vec3(0.0f, 0.0f, 3.0f);
	PT::Camera* Camera = new PT::Camera(CameraStart);

	if (!Renderer || !Shader || !Camera)
	{
		Deinitialize(Renderer, Shader, Camera);

		return 0;
	}

	if (!Renderer->SetupWindow(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		Deinitialize(Renderer, Shader, Camera);

		return 0;
	}

	Shader->LoadShader("shader.vert", PT::EShaderType::Vertex);
	Shader->LoadShader("shader.frag", PT::EShaderType::Fragment);
	Shader->Use();

	Renderer->SetupVAO();
	Renderer->SetupVBO();
	//Renderer->SetupEBO();
	Renderer->SetupTexture("wall.jpg");
	Renderer->SetupTexture("awesomeface.png", true, true);
	Renderer->SetupVertexAttrib();
	Renderer->UnbindVAO();

	Shader->SetUniformInt("Texture0", 0);
	Shader->SetUniformInt("Texture1", 1);

	glm::mat4 Transform = glm::mat4(1.0f);
	Transform = glm::translate(Transform, glm::vec3(0.5f, -0.5f, 0.0f));
	Transform = glm::rotate(Transform, glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
	Transform = glm::scale(Transform, glm::vec3(0.5f, 0.5f, 0.5f));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Set wireframe

	std::chrono::steady_clock::time_point CurrentTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point StartTime = std::chrono::high_resolution_clock::now();
	float DeltaTime = 0.0f;
	float DeltaTimeIncr = 0.0f;

	unsigned char KeyState = 0; //W = 0, A = 1, S = 2, D = 3

	bool bIsRelativeMouseMode = true;

	bool bIsRunning = true;
	while (bIsRunning)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (!bIsRelativeMouseMode)
			{
				ImGui_ImplSDL3_ProcessEvent(&event);
			}

			if (event.type == SDL_EVENT_QUIT || (event.key.key == SDLK_ESCAPE && event.key.down))
			{
				bIsRunning = false;
			}

			if (event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				Renderer->ResizeWindow(event.window.data1, event.window.data2);
			}

			//Should get a proper input class
			if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
			{
				if (event.key.key == SDLK_F1)
				{
					if (event.key.down && !event.key.repeat)
					{
						bIsRelativeMouseMode = !bIsRelativeMouseMode;
						Renderer->SetRelativeMouseMode(bIsRelativeMouseMode);
					}
				}

				bool bIsKeyStateSet = false;

				if (event.key.key == SDLK_W)
				{
					bIsKeyStateSet = KeyState & KEY_W;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_W);
					}
				}

				if (event.key.key == SDLK_A)
				{
					bIsKeyStateSet = KeyState & KEY_A;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_A);
					}
				}

				if (event.key.key == SDLK_S)
				{
					bIsKeyStateSet = KeyState & KEY_S;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_S);
					}
				}

				if (event.key.key == SDLK_D)
				{
					bIsKeyStateSet = KeyState & KEY_D;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_D);
					}
				}
			}

			if (event.type == SDL_EVENT_MOUSE_MOTION && bIsRelativeMouseMode)
			{
				Camera->UpdateCameraDirection(event.motion, DeltaTime);
			}
		}

		Renderer->ImGuiNewFrame();
		ImGui::ShowDemoWindow();

		Camera->UpdateCameraPosition(KeyState, DeltaTime);

		Shader->Use();

		uint32_t ViewID = glGetUniformLocation(Shader->GetShaderProgram(), "View");
		const glm::mat4& View = Camera->CalculateViewMatrix();
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(View));

		uint32_t ProjectionID = glGetUniformLocation(Shader->GetShaderProgram(), "Projection");
		glm::mat4 Projection;
		Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

		Renderer->BindVAO();
		for (unsigned int i = 0; i < 10; i++)
		{
			uint32_t ModelID = glGetUniformLocation(Shader->GetShaderProgram(), "Model");
			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, cubePositions[i]);
			Model = glm::rotate(Model, DeltaTimeIncr, glm::vec3(0.5f, 1.0f, 0.0f));
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		Renderer->UnbindVAO();

		Renderer->ImGuiEndFrame();
		Renderer->SwapWindow();

		std::chrono::steady_clock::time_point NewCurrentTime = std::chrono::high_resolution_clock::now();
		DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(NewCurrentTime - CurrentTime).count();
		DeltaTimeIncr += DeltaTime;
		CurrentTime = NewCurrentTime;
	}

	Deinitialize(Renderer, Shader, Camera);

	return 0;
}