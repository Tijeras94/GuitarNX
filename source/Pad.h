#ifndef PAD_H
#define PAD_H
#include <SDL.h>
#include <map>
 
#if __SWITCH__
#define KEYTYPE  Uint8
#include <switch/services/hid.h>
#else
#define KEYTYPE  SDL_Keycode

enum {
	KEY_A, KEY_B, KEY_X, KEY_Y,
	KEY_LSTICK, KEY_RSTICK,
	KEY_L, KEY_R,
	KEY_ZL, KEY_ZR,
	KEY_PLUS, KEY_MINUS,
	KEY_DLEFT, KEY_DUP, KEY_DRIGHT, KEY_DDOWN,
	KEY_LSTICK_LEFT, KEY_LSTICK_UP, KEY_LSTICK_RIGHT, KEY_LSTICK_DOWN,
	KEY_RSTICK_LEFT, KEY_RSTICK_UP, KEY_RSTICK_RIGHT, KEY_RSTICK_DOWN,
	KEY_SL_LEFT, KEY_SR_LEFT, KEY_SL_RIGHT, KEY_SR_RIGHT
};

#endif


#define SCROLL_HOLD_TIME 1000
class Pad
{
private: 
	



	static std::map<KEYTYPE, bool> keys;
	static std::map<KEYTYPE, bool> kpress;
	static Uint32 t_up;
	static Uint32 t_down;
public:
	static void update(SDL_Event* evt) { 
#if __SWITCH__ 
		//joycon
		if (evt->type == SDL_JOYBUTTONDOWN) {
			if (!keys[evt->jbutton.button])
			{
				kpress[evt->jbutton.button] = true;
			}
			keys[evt->jbutton.button] = true;

			//printf("Button: %d\n", evt->jbutton.button);
		}
		else if (evt->type == SDL_JOYBUTTONUP)
		{
			keys[evt->jbutton.button] = false;
		}


#else
		if (evt->type == SDL_KEYDOWN)
		{
		
			if (!keys[evt->key.keysym.sym])
			{
				kpress[evt->key.keysym.sym] = true;
			}
			keys[evt->key.keysym.sym] = true; 
		}
		else if (evt->type == SDL_KEYUP)
		{
			keys[evt->key.keysym.sym] = false;
		} 

#endif
		if (!up()) {
			t_up = SDL_GetTicks();
		}
		if (!down()) {
			t_down = SDL_GetTicks();
		}
	}
	 
	static bool oneHit(KEYTYPE ky) {
		if (kpress[ky] && keys[ky])
		{
			kpress[ky] = false;
			return true;
		}

		return false;
	}
	static bool up() {
		return keys[SDLK_w] | keys[13];
	} 

	static bool one_up() {
		return oneHit(SDLK_w) | oneHit(13);
	}

	static bool down() {
		return keys[SDLK_s] || keys[15];
	}

	static bool one_down() {
		return oneHit(SDLK_s) || oneHit(15);
	}

	static bool one_left() {
		return oneHit(SDLK_a) || oneHit(12);
	}

	 
	static bool left() {
		return keys[SDLK_a] | keys[12];
	}

	static bool one_right() {
		return oneHit(SDLK_d) || oneHit(14);
	}
	static bool right() {
		return keys[SDLK_d] || keys[14];
	}
	static bool cross() {
		return keys[SDLK_k] || keys[1];
	}
	static bool circle() {
		return keys[SDLK_l] || keys[0];
	}
	static bool square() {
		return keys[SDLK_j] || keys[3];
	}
	static  bool triangle() {
		return keys[SDLK_i] || keys[2];
	} 

	static  bool start() {
		return keys[SDLK_p] || keys[10];
	}


	static  bool one_start() {
		return oneHit(SDLK_p) || oneHit(10);
	}

	static  bool one_cross() { 
		return oneHit(SDLK_k) || oneHit(1);
	}
	static  bool one_circle() { 
		return oneHit(SDLK_l) || oneHit(0);
	}
	static  bool one_triangle() {
		return oneHit(SDLK_i) || oneHit(2);
	}

	static bool scroll_up() {
		if (one_up()) {
			return true;
		}
		return up() && ((SDL_GetTicks() - t_up) > SCROLL_HOLD_TIME);
	}
	static bool scroll_down() {
		if (one_down()) {
			return true;
		}
		return down() && ((SDL_GetTicks() - t_down) > SCROLL_HOLD_TIME);
	}
	 
};
#endif