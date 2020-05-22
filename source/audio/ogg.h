#pragma once

#include <SDL.h> 
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h> 
#include <iostream>
 
static ov_callbacks OV_CALLBACKS_DEFAULT = {
  (size_t(*)(void*, size_t, size_t, void*))  fread,

#ifdef __MINGW32__ 
	(int (*)(void*, ogg_int64_t, int))           fseeko64,
#elif defined (_WIN32)
	(int (*)(void*, ogg_int64_t, int))           _fseeki64, // _ov_header_fseek_wrap,
#else
	 (int (*)(void*, ogg_int64_t, int))           fseek,
#endif

  (int (*)(void*))                             fclose,
  (long (*)(void*))                            ftell
};


int open_ogg_file(const char* filename, OggVorbis_File* vf) {
	
	
	FILE* f = fopen(filename, "rb");
	if (f != NULL) {
		if (ov_open_callbacks(f, vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
			fclose(f);
			return -1;
		}
		else {
			auto info = ov_info(vf, -1);
			auto comment = ov_comment(vf, -1);
			return 0;
		}
	}
	else {
		return -1;
	}

	/*
	auto pFile = fopen(filename, "rb+");

	if (pFile == NULL) {
		printf(SDL_GetError());
		printf("\n");
		return -1;
	}
	
	printf("open_ogg_file : setting ov_callbacks\n");
	if (ov_open_callbacks(pFile, vf, NULL, 0, OV_CALLBACKS_DEFAULT) != 0)
	{
		SDL_SetError("OGG bitstream is not valid Vorbis stream!");
		return -1;
	}
	printf("open_ogg_file : done setting ov_callbacks\n");

	vorbis_info* info = ov_info(vf, -1);
	printf("open_ogg_file : done\n");
	return 0;
	*/
}
 
int ov_read_block(OggVorbis_File* vf, char* Destination, int len) {
	// A zero buffer size means decompress the entire ogg vorbis stream to PCM.
	int TotalBytesRead = 0;
	int BytesToRead = len;

	while (TotalBytesRead < BytesToRead)
	{
		//if end of file
		if (vf->offset == vf->end) {
			return 0;
		}
		long BytesRead = ov_read(vf, Destination, BytesToRead - TotalBytesRead, 0);

		if (BytesRead < 0)
		{
			// indicates an error - fill remainder of buffer with zero
			memset(Destination, 0, BytesToRead - TotalBytesRead);
			return TotalBytesRead;
		}

		TotalBytesRead += BytesRead;
		Destination += BytesRead;
	}


	Destination -= TotalBytesRead;
	return TotalBytesRead;
}