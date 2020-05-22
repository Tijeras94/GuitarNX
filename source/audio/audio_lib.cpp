#include "audio_lib.h"  
#include <iostream>
 
/* queue functions start*/

struct node
{
	Uint8* data;
	struct node* next;
};
typedef struct node node;

struct queue
{
	int count;
	node* front;
	node* rear;
};
typedef struct queue queue;



void initialize(queue* q)
{
	q->count = 0;
	q->front = NULL;
	q->rear = NULL;
}

int isempty(queue* q)
{
	return (q->rear == NULL) || (q->front == NULL);
}

void enqueue(queue* q, Uint8* value)
{
	if (q->count < FULL)
	{
		node* tmp;
		tmp = (node*)malloc(sizeof(node));
		tmp->data = value;
		tmp->next = NULL;
		if (!isempty(q))
		{
			q->rear->next = tmp;
			q->rear = tmp;
		}
		else
		{
			q->front = q->rear = tmp;
		}
		q->count++;
	}
	else
	{
		printf("List is full\n");
	}
}

Uint8* dequeue(queue* q)
{
	node* tmp;
	Uint8* n = q->front->data;
	tmp = q->front;
	q->front = q->front->next;
	q->count--;
	free(tmp);
	return(n);
}

/* queue functions end*/




/* mixer functions start*/
short _mx_chan(short a, short b) {
	int tmp = (int)a + (int)b;

	tmp -= ((int)a * (int)b) / 32767;

	if (tmp > 32767) tmp = 32767;
	if (tmp < -32767) tmp = -32767;
	return tmp;

} 

// change volume of he second chunk
void _mix_audio(char* res, char* a1, char* a2, size_t len, float vol) {

	for (int i = 0; i < len; i += 4) {
		short C1A = ((a1[i + 1] & 0xFF) << 8) | (a1[i] & 0xFF);
		short C1B = ((a2[i + 1] & 0xFF) << 8) | (a2[i] & 0xFF);

		short C2A = ((a1[i + 3] & 0xFF) << 8) | (a1[i + 2] & 0xFF);
		short C2B = ((a2[i + 3] & 0xFF) << 8) | (a2[i + 2] & 0xFF);

		short C3A = _mx_chan(C1A, C2A );
		short C3B = _mx_chan(C1B * vol, C2B * vol);

		//channel 1
		res[i] = C3A & 0xFF;
		res[i + 1] = (C3A >> 8) & 0xFF;

		//channel 2
		res[i + 2] = C3B & 0xFF;
		res[i + 3] = (C3B >> 8) & 0xFF;

	}
}

/*
	Mixes Audio,  change volume of second channel if needed
	floar vol: [0.0f,1.0f] range
*/
void mix_audio(char* a1, char* a2, size_t len, float vol)
{
	_mix_audio(a1, a1, a2, len, vol);
}

/* mixer functions end*/


queue q;
struct ff vv;
bool mixer_quit = false;
SDL_Thread* thread;
 
void audio_callback(void* userdata, Uint8* stream, int len) {
	if (!isempty(&q))
	{
		auto d = dequeue(&q);
		SDL_memcpy(stream, d, len);
		delete d;
	}
	else {
		SDL_memset(stream, 0, len);
	}

}

static int mixer_thread(void* ptr)
{
	int sampbuff = 16384;

	struct ff* cll = (struct ff*)ptr;
	while (!mixer_quit) {
		
		if ((q.count >= 0) && (q.count < 1))
		{
			Uint8* data = new Uint8[sampbuff]{0};


			//Uint8* one = new Uint8[sampbuff]{ 0 };

			if (cll->one != NULL) {
				cll->one(data, sampbuff);
			}
			//Uint8* two = new Uint8[sampbuff]{ 0 };
			if (cll->two != NULL) {
				cll->two(data, sampbuff);
			}
			
			if (cll->three != NULL) {
				cll->three(data, sampbuff);
			}
			
			if (cll->four != NULL) {
				cll->four(data, sampbuff);
			}
			
			if (cll->five != NULL) {
				cll->five(data, sampbuff);
			}
			//mix_audio((char*)data, (char*)two, 16384);

			enqueue(&q, data);
			//delete one;
			//delete two;
		}
		else {
			SDL_Delay(50);
		}
	} 
	return 0;
}

void init_audio()
{ 
	initialize(&q);

	vv.one = 0;// channelOut;
	vv.two = 0; 
	vv.three = 0; 
	vv.four = 0; 
	vv.five = 0; 
	
	{
		/* Simply create a thread */
		thread = SDL_CreateThread(mixer_thread, "Mixer_Thread", (void*)&vv);
	}
	  
	SDL_AudioSpec wav_spec; // the specs of our piece of music 
							// set the callback function
	wav_spec.callback = audio_callback;
	wav_spec.userdata = 0;
	wav_spec.format = AUDIO_S16;
	wav_spec.channels = 2;
	wav_spec.freq = 44100;
	wav_spec.samples = 4096;

	/* Open the audio device */
	if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Start playing */
	SDL_PauseAudio(0);
  
	 //printf("SDL_CreateThread Mixer \n");
}

void audioSetChannelCallback(int channel, AudioCallback_t  callback, void* pdata) {
	switch ((channel))
	{
	case 0:
		vv.one = callback;
		break;
	case 1:
		vv.two = callback;
		break;
	case 2:
		vv.three = callback;
		break;
	case 3:
		vv.four = callback;
		break;
	case 4:
		vv.five = callback;
		break;
	default:
		break;
	}
}

void end_audio() {
	//kill mixer
	mixer_quit = true;
	SDL_WaitThread(thread, 0);
	SDL_CloseAudio();
}