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

glm::vec3 cubePositions[10] = {
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

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

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

	PT::Shader CubeShader;
	CubeShader.LoadShader("cube.vert", PT::EShaderType::Vertex);
	CubeShader.LoadShader("cube.frag", PT::EShaderType::Fragment);

	PT::Shader LightShader;
	LightShader.LoadShader("light.vert", PT::EShaderType::Vertex);
	LightShader.LoadShader("light.frag", PT::EShaderType::Fragment);

	std::vector<PT::Shader*> Shaders;
	Shaders.push_back(&CubeShader);
	Shaders.push_back(&LightShader);

	uint32_t CubeVAOId = Renderer->SetupVAO();
	uint32_t CubeVBOId = Renderer->SetupVBO();

	uint32_t TexID = Renderer->SetupTexture("container.png", true);
	uint32_t SpecID = Renderer->SetupTexture("container_specular.png", true);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	Renderer->UnbindVAO();

	uint32_t LightVAOId = Renderer->SetupVAO();
	Renderer->BindVBO(CubeVBOId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	Renderer->UnbindVAO();


	std::chrono::steady_clock::time_point CurrentTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point StartTime = std::chrono::high_resolution_clock::now();
	float DeltaTime = 0.0f;
	float DeltaTimeIncr = 0.0f;

	unsigned char KeyState = 0; //W = 0, A = 1, S = 2, D = 3, E = 4, Q = 5

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
			Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f);
			glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));
		}

		//Cube setup
		Renderer->BindVAO(CubeVAOId);
		CubeShader.Use();
		CubeShader.SetUniformInt("material.diffuse", 0);
		CubeShader.SetUniformInt("material.specular", 1);
		CubeShader.SetUniformVec3("viewPos", Camera->GetCameraPos());
		CubeShader.SetUniformFloat("material.shininess", 32.0f);

		CubeShader.SetUniformVec3("dirLight.direction", glm::vec3(- 0.2f, -1.0f, -0.3f));
		CubeShader.SetUniformVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		CubeShader.SetUniformVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		CubeShader.SetUniformVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		// point light 1
		CubeShader.SetUniformVec3("pointLights[0].position", pointLightPositions[0]);
		CubeShader.SetUniformVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		CubeShader.SetUniformVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		CubeShader.SetUniformVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformFloat("pointLights[0].constant", 1.0f);
		CubeShader.SetUniformFloat("pointLights[0].linear", 0.09f);
		CubeShader.SetUniformFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		CubeShader.SetUniformVec3("pointLights[1].position", pointLightPositions[1]);
		CubeShader.SetUniformVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		CubeShader.SetUniformVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		CubeShader.SetUniformVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformFloat("pointLights[1].constant", 1.0f);
		CubeShader.SetUniformFloat("pointLights[1].linear", 0.09f);
		CubeShader.SetUniformFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		CubeShader.SetUniformVec3("pointLights[2].position", pointLightPositions[2]);
		CubeShader.SetUniformVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		CubeShader.SetUniformVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		CubeShader.SetUniformVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformFloat("pointLights[2].constant", 1.0f);
		CubeShader.SetUniformFloat("pointLights[2].linear", 0.09f);
		CubeShader.SetUniformFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		CubeShader.SetUniformVec3("pointLights[3].position", pointLightPositions[3]);
		CubeShader.SetUniformVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		CubeShader.SetUniformVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		CubeShader.SetUniformVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformFloat("pointLights[3].constant", 1.0f);
		CubeShader.SetUniformFloat("pointLights[3].linear", 0.09f);
		CubeShader.SetUniformFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		CubeShader.SetUniformVec3("spotLight.position", Camera->GetCameraPos());
		CubeShader.SetUniformVec3("spotLight.direction",Camera->GetCameraFront());
		CubeShader.SetUniformVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		CubeShader.SetUniformVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetUniformFloat("spotLight.constant", 1.0f);
		CubeShader.SetUniformFloat("spotLight.linear", 0.09f);
		CubeShader.SetUniformFloat("spotLight.quadratic", 0.032f);
		CubeShader.SetUniformFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		CubeShader.SetUniformFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		for (uint32_t i = 0; i < 10; i++)
		{
			uint32_t ModelID = glGetUniformLocation(CubeShader.GetShaderProgram(), "Model");
			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, cubePositions[i]);
			float angle = 20.0f * i;
			Model = glm::rotate(Model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		Renderer->UnbindVAO();

		//Light source setup
		Renderer->BindVAO(LightVAOId);
		LightShader.Use();

		for (uint32_t i = 0; i < 4; i++)
		{
			uint32_t ModelID = glGetUniformLocation(LightShader.GetShaderProgram(), "Model");
			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, pointLightPositions[i]);
			Model = glm::scale(Model, glm::vec3(0.2f));
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		Renderer->UnbindVAO();

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