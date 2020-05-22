#ifndef PCAMERA_H
#define PCAMERA_H

#include "ViewObject.h"

class PCamera : public OTransform
{
public:
	PCamera()  {
	}

	glm::mat4 getProjection(){
		glm::mat4 projection = glm::identity<glm::mat4>();
		projection = glm::perspective(55.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
		return projection;
	}

	glm::mat4 getPV() {
		return getProjection() * getTransform();
	}
	 
};
#endif