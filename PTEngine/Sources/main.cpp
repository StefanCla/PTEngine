#include <cstdio>
#include <SDL3/SDL.h>
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/peerinterface.h"
#include "slikenet/Gets.h"
#include "slikenet/Kbhit.h"

static SDL_Window* Window = nullptr;
static SDL_Renderer* Renderer = nullptr;

int main()
{
	SLNet::RakPeerInterface* Interface = SLNet::RakPeerInterface::GetInstance();
	
	if (Interface)
	{
		printf("SLikeNet seems to be working\n");
	}

	if (SDL_CreateWindowAndRenderer("PTEngine", 640, 480, 0, &Window, &Renderer))
	{
		printf("SDL Seems to be working\n");
	}

	printf("Hello World\n");

	printf("Hello World2\n");

	printf("Hello World3\n");

	return 0;
}