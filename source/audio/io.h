#pragma once
#include <iostream>

#define PI 3.14159265

struct AOBJ {
	char* buff;
	size_t len;
};

int readFile(const char* path, struct AOBJ* data) {
	FILE* pFile;
	pFile = fopen(path, "rb");

	if (pFile != NULL)
	{
		//get size
		long filesize;
		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);
		rewind(pFile);

		data->len = filesize;
		data->buff = (char*)malloc(sizeof(char) * filesize);
		size_t rd = fread(data->buff, sizeof(char), filesize, pFile);

		fclose(pFile);

		if (rd == filesize) {
			return 0;
		}
	}

	return -1;
}

int writeFile(const char* path, const struct AOBJ* data) {
	FILE* pFile;
	pFile = fopen(path, "wb");

	if (pFile != NULL)
	{
		size_t rd = fwrite(data->buff, sizeof(char), data->len, pFile);
		if (rd == data->len) {
			return 0;
		}
		fclose(pFile);
	}
	return -1;
}

int create_aobj(size_t len, struct AOBJ* data) {
	data->len = len;
	data->buff = (char*)malloc(sizeof(char) * data->len);

	if (data->buff == NULL) return -1;

	return 0;
}

