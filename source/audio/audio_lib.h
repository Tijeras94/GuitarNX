#pragma once
#include <SDL.h> 

#define FULL 25
//  	int bytespsec = ((44100 * 16 * 2 * 125) / 1000);



typedef void (SDLCALL* AudioCallback_t) (Uint8* stream,
	unsigned int len);

struct ff {
	AudioCallback_t one;
	AudioCallback_t two;
	AudioCallback_t three;
	AudioCallback_t four;
	AudioCallback_t five;
};

void init_audio();
void end_audio();
void mix_audio(char* a1, char* a2, size_t len, float vol = 1.0f);
void audioSetChannelCallback(int channel, AudioCallback_t  callback, void* pdata);