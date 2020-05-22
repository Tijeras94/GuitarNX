#ifndef RAWMODEL_H
#define RAWMODEL_H

#include "ViewObject.h"
#include "Renderer.h"
#include "TextureLoader.h"

class RawModel : public OTransform
{
private:
	int vaoID;
	int vertexCount;
public:

	RawModel() {}

	RawModel(int vaoID, int vertexCount) {
		this->vaoID = vaoID;
		this->vertexCount = vertexCount;
	}

	/**
	 * @return The ID of the VAO which contains the data about all the geometry
	 *         of this model.
	 */
	int getVaoID() {
		return vaoID;
	}

	/**
	 * @return The number of vertices in the model.
	 */
	int getVertexCount() {
		return vertexCount;
	}


};


class TexturedModel
{
private:
	RawModel model;
	unsigned int texture;
public:
	TexturedModel() {}
	TexturedModel(RawModel model, unsigned int texture) {
		this->model = model;
		this->texture = texture;
	}

	RawModel& getModel() {
		return model;
	}

	void setTexture(unsigned int texture) {
		this->texture = texture;

	}
	unsigned int getTexture() {
		return texture;
	}

	glm::mat4 getTransform() {
		return model.getTransform();
	}
};


class Loader {
public:
	static RawModel loadToVAO(float pos[], int posize, float textcord[], int textsize, int indices[], int indsize) {
		 
		auto VAO = createVAO();

		if(indsize != 0)
			bindIndicesBuffer(indices, indsize);

		storeDataInAttributeList(0, 3, pos, posize);
		if (textsize != 0)
			storeDataInAttributeList(1, 2, textcord, textsize);

		unbindVAO();
		if (indsize != 0) {
			RawModel model(VAO, (indsize / sizeof(int)));
			return model;
		}
		else {
			RawModel model(VAO, (posize / sizeof(float)) / 3);
			return model;
		}
		
	}

	static unsigned int loadTexture(const char* dir, const char* filename) {
		char path[200];
		snprintf(path, sizeof(path), "%s%s", dir, filename);

		auto textID = TextureLoader::getTexture(path);
		return textID;
	}

	static unsigned int loadTexture(const char* path) {
		auto textID = TextureLoader::getTexture(path);
		return textID;
	}

	static void bindIndicesBuffer(int indices[], int indsize) {
		unsigned int vboId;
		glGenBuffers(1, &vboId);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId); 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, indices, GL_STATIC_DRAW);
	}

	static void storeDataInAttributeList(int attributeNumber,int cordsize, float pos[], int posize)
	{
		auto VBO = createVBO();
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, posize, pos, GL_STATIC_DRAW);

		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(attributeNumber, cordsize, GL_FLOAT, GL_FALSE, cordsize * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	unsigned int static createVBO() {
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		return VBO;
	}

	unsigned int static createVAO() {
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		return VAO;
	}

	 void static unbindVAO() {
		glBindVertexArray(0);
	}
	  
};

#endif