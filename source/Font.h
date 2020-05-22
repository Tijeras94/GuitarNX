#ifndef FONT_H
#define FONT_H


#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>

#include <Shader.h>
#include <Renderer.h>
#include "Entry.h" 
#include <ft2build.h>
#include FT_FREETYPE_H

class Font {
private:
	FT_Library  library;
	struct Character {
		GLuint     TextureID;  // ID handle of the glyph texture
		glm::ivec2 Size;       // Size of glyph
		glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
		GLuint     Advance;    // Offset to advance to next glyph
	};

	FT_Face     face;
	std::map<GLchar, Character> chars;
	TexturedModel model;
	Shader* shader;
	Renderer render;
	glm::mat4 projection;

	FT_UInt  _fontHight;
public:

	Font(Renderer render, glm::mat4 projection) : render(render), projection(projection), face(0) {

		const char* vpgr = "#version 330 core\n"
			"layout(location = 0) in vec4 vertex;\n"
			"layout(location = 1) in vec2 textcord;\n"
			"out vec2 TexCoords;\n"
			"uniform mat4 projection;\n"
			"void main(){\n"
			"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
			"TexCoords = textcord;\n"
			"}\n";

		const char* fpgr = "#version 330 core\n"
			"in vec2 TexCoords;\n"
			"out vec4 color;\n"
			"uniform sampler2D text;\n"
			"uniform vec4 textColor;\n"
			"void main(){\n"
			"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
			"color = textColor * sampled;\n"
			"}\n";


		shader = new Shader(vpgr, fpgr, false);


		// just for init, it get overwriten by load font 
		_fontHight = 28;

		if (FT_Init_FreeType(&library))
		{
			printf("Error on Freetype Init :( \n");
		}

		float fontverts[] = {
			0.0f, 1.0f,  0.0f,
			0.0f, 0.0f,  0.0f,
			1.0f, 0.0f,  0.0f,
			1.0f, 1.0f,  0.0f,
		};
		float fontcords[] = {
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};
		int fontind[] = {
			0, 1, 2,
			0, 2, 3
		};
		model = TexturedModel(Loader::loadToVAO(fontverts, sizeof(fontverts), fontcords, sizeof(fontcords), fontind, sizeof(fontind)), 0);

	}

	int getFontHeight() {
		return _fontHight;
	}

	int loadFont(const char* dir, const char* name, FT_UInt  pixel_width, FT_UInt  pixel_height) {

		char fname[255];
		sprintf(fname, "%s%s", dir, name);

		return loadFont(fname, pixel_width, pixel_height);
	}

	int loadFont(const char* fontpath, FT_UInt, FT_UInt  pixel_height) {
		auto error = FT_New_Face(library, fontpath, 0, &face);
		_fontHight = pixel_height;
		if (error == FT_Err_Unknown_File_Format)
		{
			return -1;
		}
		else if (error)
		{
			return -1;

		}

		FT_Set_Pixel_Sizes(face, 0, pixel_height);
		loadCharTextures();
		return 0;
	}

	void Print(float x, float y, glm::vec4 color, const char* text) {
		RenderText(text, x, y, 1, color);
	}

	void Printf(float x, float y, glm::vec4 color, const char* text, ...) {

		char buffer[256];
		va_list ap;

		va_start(ap, text);
		vsnprintf(buffer, 256, text, ap);
		va_end(ap);
		buffer[255] = 0;

		Print(x, y, color, buffer);
	}

	void Print(float x, float y, bool hightLight, const char* text) {
		auto col = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
		if (hightLight) {

			col = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		RenderText(text, x, y, 1, col);
	}

	void Printf(float x, float y, bool hightLight, const char* text, ...) {

		char buffer[256];
		va_list ap;

		va_start(ap, text);
		vsnprintf(buffer, 256, text, ap);
		va_end(ap);
		buffer[255] = 0;

		Print(x, y, hightLight, buffer);
	}

	int mesureText(const char* text, GLfloat scale = 1) {
		int x = 0;
		for (size_t i = 0; i < strlen(text); i++)
		{
			auto ch = chars.at('~');
			if (text[i] >= 0)
				ch = chars.at(text[i]);

			GLfloat xpos = x + ch.Bearing.x * scale;

			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		return x;
	}

	/*
		*displays a text on the screen with carriage return
		*
		* @param x the x position of the text on the screen
		* @param y the position y of the text on the screen
		* @param xmax the maximum length of a line
		* @param yjump the height of a line break
		* @param hightLight true if the text must be overwritten
		* @param text the text to display
	*/
	void fontPrintRC(float x, float y, float xmax, float yjump, bool hightLight, const char* text) {
		char buff[128];
		int pos = 0;
		for (int i = 0; text[i] != 0; i++) {
			buff[pos] = text[i];
			buff[pos + 1] = 0;
			pos++;

			if (mesureText(buff) > xmax) {
				buff[pos] = 0;

				//for (int j = 0; j < 15; j++) {
				//	if (text[i - j] == ' ') {
				//		buff[pos - j] = 0;
				//		i -= j;
				//		break;
				//	}
				//}

				Print(x, y, hightLight, buff);
				y -= yjump;
				pos = 0;
			}
			else if (text[i + 1] == 0) {
				Print(x, y, hightLight, buff);
			}
			else {
				//Print(x, y, hightLight, buff);
				//break;
			}
		}
	}

	void printRCf(const char* text, ...) {
		char buffer[256];
		va_list ap;

		va_start(ap, text);
		vsnprintf(buffer, 256, text, ap);
		va_end(ap);
		buffer[255] = 0;

		printRC(buffer);
	}

	void printRC(const char* text) {
		fontPrintRC(10, SCREEN_HEIGHT / 2, SCREEN_WIDTH - getFontHeight(), getFontHeight(), 1, text);
	}

private:
	void RenderText(const char* text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color = glm::vec4(0.3, 0.7f, 0.9f, 1.0f))
	{
		shader->use();
		for (size_t i = 0; i < strlen(text); i++)
		{
			auto ch = chars.at('~');   
			if (text[i] == '\t' || text[i] == '\n' || text[i] == '\r') {
				ch = chars.at(' ');
			}else if ((text[i] >= 0) && (text[i] < 128))
				ch = chars.at(text[i]); 

			model.setTexture(ch.TextureID);
			auto mtans = model.getTransform();

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			mtans = glm::translate(mtans, glm::vec3(xpos, ypos, 0));
			mtans = glm::scale(mtans, glm::vec3(ch.Size.x, ch.Size.y, 0));
			shader->setVec4("textColor", color);
			shader->setMat4("projection", projection * mtans);
			render.render(model);

			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
	}

	void loadCharTextures() {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			chars.insert(std::pair<GLchar, Character>(c, character));
		}
	}

};

class TextPrinter {

private:
	Font* __font;
	Font* __font2;
	Font* __curFont;

	float textMotion;
	float ysize;
	float fontScale;


	float fontHightlightZoom;
	float fontHeight;
	float fontSpace;
	float textMotionVelociy;

public:
	TextPrinter(Font* normal, Font* highlight) : __font(normal), __font2(highlight), __curFont(normal), fontScale(1.0f) {
		resetMotionMenu();
		fontHightlightZoom = 1.0f;
		fontHeight = highlight->getFontHeight();
		fontSpace = 2.0f;
		textMotionVelociy = 0.3f;
	}

	void resetMotionMenu() {
		textMotion = 0.0f;
	}

	void motionUp() {
		textMotion -= ysize + fontSpace;
	}

	void motionDown() {
		textMotion += ysize + fontSpace;
	}

	void printMenu(std::vector<char*> menu, int index) {
		__curFont = __font;

		textMotion *= textMotionVelociy;

		ysize = fontHeight;
		auto ss = SCREEN_HEIGHT / 2;
		float basea = ss - fontHightlightZoom * ysize / 2 - fontSpace + textMotion + 100;
		float baseb = ss + fontHightlightZoom * ysize / 2 + textMotion;

		int i = 0;
		for (std::vector<char*>::iterator it = menu.begin(); it < menu.end(); it++) {
			char* txt = *it;
			printMenuEntry(basea, baseb, i - index, txt);
			i++;
		}
	}

	void printMenuF(FolderEntry* menu) {
		__curFont = __font;

		if (menu->list->empty()) {
			__curFont->Printf(10, fontHeight, true, "empty folder");
			return;
		}
		int index = menu->selection;
		textMotion *= textMotionVelociy;

		ysize = 2 * fontHeight + fontSpace;

		auto ss = (SCREEN_HEIGHT / 2) - ysize;
		float basea = ss + fontHightlightZoom * ysize / 2 - fontSpace + textMotion + 110;
		float baseb = ss + fontHightlightZoom * ysize / 2 + textMotion;

		char aname[128];
		char sname[128];
		for (int i = ((index - 10) > 0 ? (index - 10) : 0); (i < (index + 10)) && (i < menu->list->size()); i++) {

			if (menu->list->at(i)->type == FOLDER_ENTRY) {
				FolderEntry* fentry = (FolderEntry*)menu->list->at(i);
				sprintf(aname, "%s", fentry->name);
				sprintf(sname,/*"dossier"*/ "folder");
			}
			else if (menu->list->at(i)->type == MUSIC_ENTRY) {
				MusicEntry* mentry = (MusicEntry*)menu->list->at(i);
				sprintf(aname, "%s", mentry->name);
				sprintf(sname, "%s", mentry->artist);
			}

			if (i - index < 0) {
				if ((-1 * (i - index)) > 2)
					continue;
				// top items
				auto ycord = basea + (-1 * (i - index + 1)) * (fontSpace + ysize) + 30;
				__curFont->Printf(10, ycord + fontHeight + fontSpace, false, aname);
				__curFont->Printf(10, ycord, false, sname);

			}
			else if (i == index) {
				__curFont = __font2;
				__curFont->Printf(10, baseb, true, sname);
				__curFont->Printf(10, baseb + fontHightlightZoom * fontHeight - fontSpace, true, aname);
				__curFont = __font;
			}
			else {
				if (((i - index)) > 2)
					continue;
				//bottom items
				auto ycord = baseb - (i - index) * (fontSpace + ysize) + (60 - 30);
				__curFont->Printf(10, ycord - fontHeight - fontSpace, false, sname);
				__curFont->Printf(10, ycord, false, aname);
			}
		}
	}

private:
	void printMenuEntry(float basea, float baseb, int row, char* txt) {
		if (row < 0) {
			__curFont->Printf(25, baseb - row * (fontSpace + ysize), false, txt);

		}
		else if (row == 0) {
			__curFont = __font2;
			__curFont->Printf(25, baseb - row * (fontSpace + ysize), true, txt);
			__curFont = __font;
		}
		else {
			__curFont->Printf(25, basea - (row + 1) * (fontSpace + ysize) + 30, false, txt);
		}
	}

};
#endif