#include "precomp.h"

#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"

#include "graphics/renderer.h"
#include "graphics/shader.h"

int main()
{
	PT::Renderer* Renderer = new(PT::Renderer);
	PT::Shader* Shader = new(PT::Shader);

	if (Renderer)
	{
		if (Renderer->SetupWindow(640, 480))
		{
			Shader->LoadShader("shader.vert", PT::EShaderType::Vertex);
			Shader->LoadShader("shader.frag", PT::EShaderType::Fragment);
			Shader->Use();

			Renderer->SetupVAO();
			Renderer->SetupVBO();
			Renderer->SetupEBO();
			Renderer->SetupTexture();
			Renderer->SetupVertexAttrib();
			Renderer->UnbindVAO();

			Shader->SetUniformInt("Texture0", 0);
			Shader->SetUniformInt("Texture1", 1);

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Set wireframe

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
						Renderer->ResizeWindow(event.window.data1, event.window.data2);
					}
				}

				Shader->Use();
				Renderer->BindVAO();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				Renderer->UnbindVAO();

				Renderer->SwapWindow();
			}
		}
	}

	delete Renderer;
	Renderer = nullptr;

	delete Shader;
	Shader = nullptr;

	return 0;
}