/* object_list.h
* Computer Graphics 2017
* Author : Lee Sora
* Author : Cho Youngwoo
* Author : Choi Moonyoung
* 2017.06.12
*/

#ifndef OBJECT_H
#define OBJECT_H 

#include <stdio.h>
#include <vector>
#include <queue>

#include <glm\glm.hpp>
#include <glm\gtc/matrix_transform.hpp>

#include "camera.h"
#include "WaveFrontOBJ.h"

class Object {

public:

	int render1[50000];
	int render2[50000];
	GLuint EBO;
	GLuint FBO;
	GLuint nVBO;
	GLuint vertexBuffer;
	GLuint renderBuffer1;
	GLuint renderBuffer2;
	WaveFrontOBJ * object;
	glm::mat4 modelview_matrix;
	std::vector <glm::vec3> bs_pos;
	std::vector <GLfloat> bs_radius;

	std::queue <int> renderQueue;

	Object(char * FILENAME);
	void initObject();
	void drawObject1();
	void drawObject2();
	void clearObject();
	void calcBoundingSphere(int num);
	void setPos2D(glm::vec2 v);
	void setPos2D(GLfloat x, GLfloat z);
	void setPos3D(glm::vec3 v);
	void setPos3D(GLfloat x, GLfloat y, GLfloat z);
	void traceCamera(Camera c);
	void moveObject(glm::vec3 v);
	void moveObject(GLfloat x, GLfloat y, GLfloat z);
	void rotateObject(GLfloat angle, glm::vec3 v);
	void rotateObject(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	void scaleObject(GLfloat ratio, glm::vec3 v);
	void scaleObject(GLfloat ratio, GLfloat x, GLfloat y, GLfloat z);
	glm::mat4 modelview();
	void initQueue(int spot);
	void updateQueue();
};

#endif