#include "precomp.h"

#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"
	
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

			uint32_t TransformID = glGetUniformLocation(Shader->GetShaderProgram(), "Transform");

			uint32_t ModelID = glGetUniformLocation(Shader->GetShaderProgram(), "Model");
			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::rotate(Model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));	
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));

			uint32_t ViewID = glGetUniformLocation(Shader->GetShaderProgram(), "View");
			glm::mat4 View = glm::mat4(1.0f);
			View = glm::translate(View, glm::vec3(0.0f, 0.0f, -3.0f));
			glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(View));

			uint32_t ProjectionID = glGetUniformLocation(Shader->GetShaderProgram(), "Projection");
			glm::mat4 Projection;
			Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
			glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));



			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Set wireframe

			bool bIsRunning = true;
			while (bIsRunning)
			{
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					if (event.type == SDL_EVENT_QUIT || (event.key.key == SDLK_ESCAPE && event.key.down))
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
				//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glDrawArrays(GL_TRIANGLES, 0, 36);
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