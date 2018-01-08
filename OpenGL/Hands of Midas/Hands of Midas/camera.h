/* camera.h
* Computer Graphics 2017
* Author : Lee Sora
* Author : Cho Youngwoo
* Author : Choi Moonyoung
* 2017.06.12
*/

#ifndef CAMERA_H
#define CAMERA_H 

#include <glm\glm.hpp>
#include <glm\gtc/matrix_transform.hpp>

class Camera {

public:

	GLfloat v;
	GLfloat w;
	GLfloat radius;
	GLfloat angle;
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;
	bool fix;

		Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat targetX, GLfloat targetY, GLfloat targetZ, GLfloat upX, GLfloat upY, GLfloat upZ);
		Camera (GLfloat r, GLfloat a);

		void moveForward();
		void moveBackward();
		void moveRight();
		void moveLeft();
		void refresh();
		void stop();
		glm::mat4 lookAt();

private:

		void _refresh();
};

#endif