#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#include "Renderer.h"
#include "stb_image.h"

class TextureLoader {
public:
	static unsigned int getTexture(const char*path) {
		unsigned int s_tex;
		glGenTextures(1, &s_tex);
		glBindTexture(GL_TEXTURE_2D, s_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nchan;
		stbi_set_flip_vertically_on_load(true);
		 
		auto file = fopen(path, "rb+");
		
		if (file == NULL) return 0;

		stbi_uc* img = stbi_load_from_file(file, &width, &height, &nchan, 4);
		if (img)
		{ 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			printf("Failed to load texture\n");
			return 0;
		}
		stbi_image_free(img);
		fclose(file);
		//glActiveTexture(0); // activate the texture unit first before binding texture
		return s_tex;
	}
};

#endif