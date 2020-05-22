#ifndef GNECK_H
#define GNECK_H

#include "Renderer.h"
#include "TextureLoader.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>



class GNeck
{
private:
	//diviseur de la longeur graphique des notes
	float ZOOM;
	float GRID_RESOLUTION;
	Renderer* renderer;
	Shader* texShader;
	Shader* colShader;
	RawModel mache;
	TexturedModel line;
	RawModel mancheLinesraw;


	TexturedModel meter;
	int _mpower;
	const char* skinPath;

	unsigned int meterTexts[3];
public:

	GNeck(const char* skin, Renderer* renderer, Shader* texShader, Shader* colShader) : renderer(renderer), texShader(texShader), colShader(colShader), skinPath(skin) {

		GRID_RESOLUTION = 2.0f;
		_mpower = 0;
	}

	void setZOOM(float zoom) {
		ZOOM = zoom;
	}

	void load() {
		int indices[] = {
			0,1,2,
			0,2,3
		};
		float textureCoords[] = {
			0,0,
			0,1,
			1,1,
			1,0
		};

		float _mache[] = {
					 -1.4f,  15.0f, 0.0f,
					  1.4f,  15.0f, 0.0f,
					  1.4f, -10.0f, 0.0f,
					 -1.4f, -10.0f, 0.0f
		};
		mache = Loader::loadToVAO(_mache, sizeof(_mache), 0, 0, indices, sizeof(indices));

		float mancheLines[] = {
			 -1.0f,  15.0f, 0.0f,
			 -1.0f, -10.0f, 0.0f,

			 -0.5f,  15.0f, 0.0f,
			 -0.5f, -10.0f, 0.0f,

			 -0.0f,  15.0f, 0.0f,
			 -0.0f, -10.0f, 0.0f,

			  0.5f,  15.0f, 0.0f,
			  0.5f, -10.0f, 0.0f,

			  1.0f,  15.0f, 0.0f,
			  1.0f, -10.0f, 0.0f
		};
		mancheLinesraw = Loader::loadToVAO(mancheLines, sizeof(mancheLines), 0, 0, 0, 0);

		float _line[] = {
			-1.4f,-0.1f, 0.0f,
			-1.4f, 0.1f, 0.0f,
			 1.4f, 0.1f, 0.0f,
			 1.4f,-0.1f, 0.0f
		};
		auto lineraw = Loader::loadToVAO(_line, sizeof(_line), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
		auto linetext = Loader::loadTexture(skinPath, "barrette.png");
		line = TexturedModel(lineraw, linetext);


		float _meter[] = {
			0.0f - 0.5f, 0.0f, 0.0f,
			0.0f - 0.5f, 0.25f, 0.0f,
			1.0f - 0.5f, 0.25f, 0.0f,
			1.0f - 0.5f, 0.0f, 0.0f,
		};

		auto meterraw = Loader::loadToVAO(_meter, sizeof(_meter), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
		meterTexts[0] = Loader::loadTexture(skinPath, "box.png");
		meterTexts[1] = Loader::loadTexture(skinPath, "box1.png");
		meterTexts[2] = Loader::loadTexture(skinPath, "box2.png");
		meter = TexturedModel(meterraw, meterTexts[_mpower]);
	}

	void setPower(int p) {  
		
		if (p < 33)
			p = 0;
		else if (p > 33) {
			if (p < 66)
				p = 1;
			else
				p = 2;
		}
		else {
			p = 2;
		}


		/*if (p <= 3)
			p = 0;
		else if (p == 4)
			p = 1;
		else if (p > 4)
			p = 2;*/

		_mpower = p;
	}

	void render(glm::mat4 PV, float dt) {
		texShader->use();
		texShader->setInt("textureSampler", 0);


		auto model = glm::identity<glm::mat4>();

		colShader->use();


		//neckBG
		colShader->setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
		colShader->setMat4("pmv", PV * mache.getTransform());
		renderer->render(mache);


		//neck lines
		colShader->setVec4("color", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
		colShader->setMat4("pmv", PV * mancheLinesraw.getTransform());
		renderer->renderLines(mancheLinesraw);



		texShader->use();
		texShader->setInt("textureSampler", 0);


		//metet
		meter.getModel().translateX(-1.25f);
		meter.getModel().rotateX(33.0f);
		meter.setTexture(meterTexts[_mpower]);
		colShader->setMat4("pmv", PV * meter.getTransform());
		renderer->render(meter);


		//bars
		for (float i = 0; i < 14 / GRID_RESOLUTION; i++) {
			line.getModel().translateY(
				(
					 -(
					(float)(
						(int)(dt * 100.0f / ZOOM)
						%
						(int)(1000.0f * GRID_RESOLUTION)
						)
						) / 1000.0f
					)
				+ i
				* GRID_RESOLUTION);
			texShader->setMat4("pmv", PV * line.getTransform());
			renderer->render(line);
		}
	}
};
#endif