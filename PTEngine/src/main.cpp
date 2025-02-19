#include <cstdio>
#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"

#include "graphics/renderer.h"
#include "graphics/shader.h"

int main()
{
	Renderer* Render = new(Renderer);
	Shader* Shade = new(Shader);

	if (Render)
	{
		if (Render->SetupWindow(640, 480))
		{
			Shade->LoadShader("VertexShader.glsl", EShaderType::Vertex);
			Shade->LoadShader("FragmentShader.glsl", EShaderType::Fragment);
			Shade->Use();
			//uint32_t Color = glGetUniformLocation(Shade->GetShaderProgram(), "ourColor");
			//glUniform4f(Color, 1.0, 0.0, 1.0, 0.0);

			Render->SetupVAO();
			Render->SetupVBO();
			//Render->SetupEBO();
			Render->SetupVertexAttrib();
			Render->UnbindVAO();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			bool bIsRunning = true;
			while (bIsRunning)
			{
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					if (event.type == SDL_EVENT_QUIT)
					{
						bIsRunning = false;
					}

					if (event.type == SDL_EVENT_WINDOW_RESIZED)
					{
						Render->ResizeWindow(event.window.data1, event.window.data2);
					}
				}

				glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				Shade->Use();
				Render->BindVAO();
				//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				Render->UnbindVAO();

				Render->SwapWindow();
			}
		}
	}

	delete Render;
	Render = nullptr;

	return 0;
}