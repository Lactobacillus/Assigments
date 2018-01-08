/* camera.cpp
 * Computer Graphics 2017
 * Author : Lee Sora
 * Author : Cho Youngwoo
 * Author : Choi Moonyoung
 * 2017.06.06
*/

#define MAX_V 0.1
#define MAX_W 0.2
#define FRACTION 0.005
#define CUBE_SIZE 100.0f

#include <stdio.h>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera (GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat targetX, GLfloat targetY, GLfloat targetZ, GLfloat upX, GLfloat upY, GLfloat upZ) {

	// 절대 시점 카메라

		fix = true;
		this->pos = glm::vec3(posX, posY, posZ);
		this->target = glm::vec3(targetX, targetY, targetZ);
		this->up = glm::vec3(upX, upY, upZ);
}

Camera::Camera(GLfloat r, GLfloat a) {

	// 1인칭 시점 카메라

		fix = false;
		this->v = 0;
		this->w = 0;
		this->radius = r;
		this->angle = a;
		this->pos = glm::vec3(radius * glm::sin(glm::radians(this->angle)), 6.0, radius * glm::cos(glm::radians(this->angle)));
		this->target = glm::vec3(0.0, 6.0, 0.0);
		this->up = glm::vec3(0.0, 1.0, 0.0);
}

void Camera::moveForward () {

	// 카메라가 앞으로 가는 경우
	// 가속도를 줌

		if (!fix) {

			if (glm::abs(this->v) < MAX_V) {

				this->v = this->v - (GLfloat) 0.01;
			}
			else {

				this->v = (GLfloat) -MAX_V;
			}

			if (this->radius < 15.0) {

				this->stop();
			}

			this->_refresh();
		}
}

void Camera::moveBackward (){

	// 카메라가 뒤로 가는 경우
	// 가속도를 줌

		if (!fix) {

			if (glm::abs(this->v) < MAX_V) {

				this->v = this->v + (GLfloat) 0.01;
			}
			else {

				this->v = (GLfloat) MAX_V;
			}

			if (this->radius > 100.0) {

				this->stop();
			}

			this->_refresh();
		}
}

void Camera::moveRight () {

	// 카메라가 오른쪽으로 가는 경우
	// 각가속도를 줌

		if (!fix) {

			if (glm::abs(this->w) < MAX_W) {

				this->w = this->w + (GLfloat) 0.01;
			}
			else {

				this->w = (GLfloat) MAX_W;
			}

			this->_refresh();
		}
}

void Camera::moveLeft () {

	// 카메라가 뒤로 가는 경우
	// 각가속도를 줌

		if (!fix) {

			if (glm::abs(this->w) < MAX_W) {

				this->w = this->w - (GLfloat) 0.01;
			}
			else {

				this->w = (GLfloat) -MAX_W;
			}

			this->_refresh();
		}
}

void Camera::refresh () {

	GLfloat time = 1.0;

		if (!fix) {

			this->_refresh();
			this->v = (GLfloat)(1.0 - FRACTION) * this->v;
			this->w = (GLfloat)(1.0 - FRACTION) * this->w;
		}
}

void Camera::stop() {

	// 카메라의 움직임을 멈춤
	// 손 1 인칭 시점에서 사용

		this->v = 0;
		this->w = 0;
}

glm::mat4 Camera::lookAt() {

	// 카메라 시점 매트릭스를 반환

	return glm::lookAt(this->pos, this->target, this->up);
}

void Camera::_refresh() {

	GLfloat time = 1.0;

		this->radius += this->v * time;
		this->angle += this->w * time;
		this->pos = glm::vec3(this->radius * glm::sin(glm::radians(this->angle)), 6, this->radius * glm::cos(glm::radians(this->angle)));
}