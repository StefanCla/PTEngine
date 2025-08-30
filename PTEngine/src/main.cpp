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
#include "graphics/model.hpp"

#include <chrono>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec3 LightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

inline void ChangeKeyState(unsigned char& KeyState, int Key)
{
	KeyState = KeyState ^ Key;
}

void Deinitialize(PT::Renderer* Renderer, PT::Camera* Camera)
{
	delete Renderer;
	Renderer = nullptr;

	delete Camera;
	Camera = nullptr;
}

int main()
{
	PT::Renderer* Renderer = new PT::Renderer;

	glm::vec3 CameraStart = glm::vec3(0.0f, 0.0f, 10.0f);
	PT::Camera* Camera = new PT::Camera(CameraStart);

	if (!Renderer || !Camera)
	{
		Deinitialize(Renderer, Camera);

		return 0;
	}

	if (!Renderer->SetupWindow(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		Deinitialize(Renderer, Camera);

		return 0;
	}

	std::vector<PT::Shader*> Shaders;

	//PT::Model model("../resources/model/backpack.obj", Camera);

	std::chrono::steady_clock::time_point CurrentTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point StartTime = std::chrono::high_resolution_clock::now();
	float DeltaTime = 0.0f;
	float DeltaTimeIncr = 0.0f;

	unsigned char KeyState = 0; //W = 0, A = 1, S = 2, D = 3, E = 4, Q = 5

	bool bIsRelativeMouseMode = true;

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

				if (event.key.key == SDLK_E)
				{
					bIsKeyStateSet = KeyState & KEY_E;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_E);
					}
				}

				if (event.key.key == SDLK_Q)
				{
					bIsKeyStateSet = KeyState & KEY_Q;
					if (event.key.down != bIsKeyStateSet)
					{
						ChangeKeyState(KeyState, KEY_Q);
					}
				}
			}

			if (event.type == SDL_EVENT_MOUSE_MOTION && bIsRelativeMouseMode)
			{
				Camera->UpdateCameraDirection(event.motion, DeltaTime);
			}
		}

		Renderer->ImGuiNewFrame();
		//ImGui::ShowDemoWindow();

		Camera->UpdateCameraPosition(KeyState, DeltaTime);

		for (PT::Shader* Shader : Shaders)
		{
			Shader->Use();

			uint32_t ViewID = glGetUniformLocation(Shader->GetShaderProgram(), "View");
			const glm::mat4& View = Camera->CalculateViewMatrix();
			glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(View));

			uint32_t ProjectionID = glGetUniformLocation(Shader->GetShaderProgram(), "Projection");
			glm::mat4 Projection;
			Projection = glm::perspective(glm::radians(65.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f);
			glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));
		}

		//model.Draw();

		Renderer->ImGuiEndFrame();
		Renderer->SwapWindow();

		std::chrono::steady_clock::time_point NewCurrentTime = std::chrono::high_resolution_clock::now();
		DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(NewCurrentTime - CurrentTime).count();
		DeltaTimeIncr += DeltaTime;
		CurrentTime = NewCurrentTime;
	}

	Deinitialize(Renderer, Camera);

	return 0;
}