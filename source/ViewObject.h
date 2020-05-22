#ifndef VIEWOBJECT_H
#define VIEWOBJECT_H

class OTransform {
private:
	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;
public:
	OTransform()  { resetTransform(); }

	void resetTransform() { _position = glm::vec3(0, 0, 0); _rotation = glm::vec3(0, 0, 0);  _scale = glm::vec3(1, 1, 1); }

	void translate(glm::vec3 positions) { _position = positions; }
	void translateX(float x) { _position = glm::vec3(x, _position.y, _position.z);}
	void translateY(float y) { _position = glm::vec3(_position.x, y, _position.z); }
	void translateZ(float z) { _position = glm::vec3(_position.x, _position.y, z); }


	void rotate(glm::vec3 angles) { _rotation = angles; }
	void rotateX(float x) { _rotation = glm::vec3(x			 , _rotation.y, _rotation.z); }
	void rotateY(float y) { _rotation = glm::vec3(_rotation.x, y		  , _rotation.z); }
	void rotateZ(float z) { _rotation = glm::vec3(_rotation.x, _rotation.y, z); }

	void scale(glm::vec3 scales) { _scale = scales; }
	void scaleX(float x) { _scale = glm::vec3(x, _scale.y, _scale.z);}
	void scaleY(float y) { _scale = glm::vec3(_scale.x, y, _scale.z); }
	void scaleZ(float z) { _scale = glm::vec3(_scale.x, _scale.y, z); }

	glm::mat4 getTransform() {

		glm::mat4 trans = glm::identity<glm::mat4>();
		trans = glm::scale(trans, _scale);
		trans = glm::rotate(trans, glm::radians(_rotation.x), glm::vec3(1, 0, 0));
		trans = glm::rotate(trans, glm::radians(_rotation.y), glm::vec3(0, 1, 0));
		trans = glm::rotate(trans, glm::radians(_rotation.z), glm::vec3(0, 0, 1));
		trans = glm::translate(trans, _position);
		return trans;
	}
};
#endif