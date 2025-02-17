#include <cstdio>
#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"

#include "graphics/renderer.h"

static SDL_Window* Window = nullptr;
static SDL_Renderer* Renderer = nullptr;

int main()
{
	SLNet::RakPeerInterface* Interface = SLNet::RakPeerInterface::GetInstance();
	
	if (Interface)
	{
		printf("SLikeNet seems to be working\n");
	}

	Renderer2* Render = new(Renderer2);

	if (Render)
	{
		if (Render->SetupWindow(640, 480))
		{
			Render->SetupVertexShader();
			Render->SetupFragmentShader();
			Render->SetupShaderProgram();

			Render->SetupVAO();
			Render->SetupVBO();
			Render->SetupEBO();
			Render->SetupVertexAttrib();
			Render->UnbindVAO();

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

				glUseProgram(Render->m_ShaderProgramID);
				Render->BindVAO();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				Render->UnbindVAO();

				Render->SwapWindow();
			}
		}
	}

	delete Render;

	return 0;
}