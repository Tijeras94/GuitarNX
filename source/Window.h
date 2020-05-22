#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <glad/glad.h>

#include "audio/audio_lib.h"
#include "Pad.h"

#ifdef __SWITCH__ 
#include <switch.h>

//-----------------------------------------------------------------------------
// nxlink support
//-----------------------------------------------------------------------------

#ifndef ENABLE_NXLINK
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

static int s_nxlinkSock = -1;

static void initNxLink()
{
	if (R_FAILED(socketInitializeDefault()))
		return;

	s_nxlinkSock = nxlinkStdio();
	if (s_nxlinkSock >= 0)
		TRACE("printf output now goes to nxlink server");
	else
		socketExit();
}

static void deinitNxLink()
{
	if (s_nxlinkSock >= 0)
	{
		close(s_nxlinkSock);
		socketExit();
		s_nxlinkSock = -1;
	}
}

extern "C" void userAppInit()
{
	initNxLink();
}

extern "C" void userAppExit()
{
	deinitNxLink();
}

#endif


#endif

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Window
{
private:
	bool isRunning; 
	SDL_Window* win;
public:

	Window(){

		// mandatory at least on switch, else gfx is not properly closed
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
			SDL_Log("SDL_Init: %s\n", SDL_GetError());
			isRunning = false;
			return;
		}

		/* Request opengl 3.2 context.
		* SDL doesn't have the ability to choose which profile at this time of writing,
		* but it should default to the core profile */
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);


		// create an SDL window (OpenGL ES2 always enabled)
		// when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
		// available switch SDL2 video modes :
		// 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
		// 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
		win = SDL_CreateWindow("OPENGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
		if (!win) {
			SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
			isRunning = false;
			return;
		}

#ifdef __SWITCH__
		// open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
		// when railed, both joycons are mapped to joystick #0,
		// else joycons are individually mapped to joystick #0, joystick #1, ...
		// https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
		for (int i = 0; i < 2; i++) {
			if (SDL_JoystickOpen(i) == NULL) {
				SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
				isRunning = false;
				break;
				return;
			}
		}
#endif;

		SDL_GLContext ctx = SDL_GL_CreateContext(win);

		gladLoadGL();

		SDL_GL_SetSwapInterval(1);

		
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		isRunning = true;


	}

	void updateDisplay() {
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			Pad::update(&evt);
#ifndef __SWITCH__
			if (evt.type == SDL_QUIT)
			{
				isRunning = false;
				break;
			}
#else
			switch (evt.type) {
			case SDL_JOYAXISMOTION:
				SDL_Log("Joystick %d axis %d value: %d\n",
					evt.jaxis.which,
					evt.jaxis.axis, evt.jaxis.value);
				break;

			case SDL_JOYBUTTONDOWN:
			 
				// https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
				// seek for joystick #0
				if (evt.jbutton.which == 0) {
					if (evt.jbutton.button == 0) {

					}
					else if (evt.jbutton.button == 10) {
						// (+) button down
						//isRunning = false;
						//break;
					}
				}
				break;

			default:
				break;
		}
#endif
		}

		SDL_GL_SwapWindow(win);
	}

	bool isClosedRequested() {
		return isRunning;
	}

	void destroy() {
		//end_audio();
		SDL_DestroyWindow(win);
		SDL_Quit();
	}


};
#endif