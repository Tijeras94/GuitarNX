#ifndef RENDERER_H
#define RENDERER_H

#include "Window.h"
#include "RawModel.h"


class Renderer
{
private:

public:

	Renderer(){ }

	void prepare() {
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void render(RawModel model) {
		glBindVertexArray(model.getVaoID());
		glEnableVertexAttribArray(0);
		//glDrawArrays(GL_TRIANGLES, 0, model.getVertexCount());
		glDrawElements(GL_TRIANGLES, model.getVertexCount(), GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

	} 

	void render(TexturedModel texModel) {
		auto model = texModel.getModel();

		glBindVertexArray(model.getVaoID());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texModel.getTexture());

		//glDrawArrays(GL_TRIANGLES, 0, model.getVertexCount());
		glDrawElements(GL_TRIANGLES, model.getVertexCount(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

	}

	void renderLines(RawModel model) {  
		glBindVertexArray(model.getVaoID());
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINES, 0, model.getVertexCount());
		//glDrawElements(GL_TRIANGLES, model.getVertexCount(), GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

	}

};
#endif