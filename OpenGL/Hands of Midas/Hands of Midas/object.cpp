/* object_list.cpp
* Computer Graphics 2017
* Author : Lee Sora
* Author : Cho Youngwoo
* Author : Choi Moonyoung
* 2017.06.12
*/

#define GLM_FORCE_CUDA
#define RENDERING 1

#include <stdio.h>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>
#include <iostream>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc/matrix_transform.hpp>
#include <glm\gtx/string_cast.hpp>

#include "camera.h"
#include "object.h"
#include "WaveFrontOBJ.h"

extern unsigned int render_mode;

Object::Object(char * FILENAME) {

	glm::mat4 temp(1.0);

		this->object = new WaveFrontOBJ(FILENAME);
		this->modelview_matrix = glm::mat4(1.0);

		for (int i = 0; i < 50000; i++) {

			render1[i] = 0;
			render2[i] = 0;
		}
}

void Object::initObject() {

		glGenBuffers(1, &this->vertexBuffer);
		glGenBuffers(1, &this->renderBuffer1);
		glGenBuffers(1, &this->renderBuffer2);
		glGenBuffers(1, &this->nVBO);
		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->object->verts.size() * sizeof(glm::vec3), &this->object->verts[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->nVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*this->object->normals.size(), &this->object->normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->renderBuffer1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(int) *object->verts.size(), &this->render1[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->renderBuffer2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(int) *object->verts.size(), &this->render2[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->object->vIndex.size() * sizeof(GLuint), &this->object->vIndex[0], GL_STATIC_DRAW);
}

void Object::drawObject1 () {

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, this->nVBO);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, this->renderBuffer1);
		glVertexAttribIPointer(
			2,
			1,
			GL_UNSIGNED_INT,
			0,
			(void*)0
			);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, this->renderBuffer2);
		glVertexAttribIPointer(
			3,
			1,
			GL_UNSIGNED_INT,
			0,
			(void*)0
			);
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glVertexAttribPointer(
			4,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
}

void Object::drawObject2 () {

	glDrawElements(GL_TRIANGLES, this->object->vIndex.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void Object::clearObject() {
		
		glDeleteBuffers(1, &(this->vertexBuffer));
		glDeleteBuffers(1, &(this->nVBO));
		glDeleteBuffers(1, &(this->renderBuffer1));
		glDeleteBuffers(1, &(this->renderBuffer2));
		glDeleteBuffers(1, &(this->EBO));
		//glDeleteBuffers(1, &uvbuffer);
		//glDeleteTextures(1, &Texture);
}

/*
void Object::calcBoundingSphere(int num) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution <> dis(0, this->object->verts.size());

		for (int i = 0; i < num; i++) {

			GLfloat d[16];
			GLfloat e[16];
			GLfloat f[16];
			GLfloat g[16];
			int idx[4] = { dis(gen), dis(gen), dis(gen), dis(gen) };

			glm::vec3 p1(this->object->verts[idx[0]].x, this->object->verts[idx[0]].y, this->object->verts[idx[0]].z);
			glm::vec3 p2(this->object->verts[idx[1]].x, this->object->verts[idx[1]].y, this->object->verts[idx[1]].z);
			glm::vec3 p3(this->object->verts[idx[2]].x, this->object->verts[idx[2]].y, this->object->verts[idx[2]].z);
			glm::vec3 p4(this->object->verts[idx[3]].x, this->object->verts[idx[3]].y, this->object->verts[idx[3]].z);
			GLfloat t1 = (p1.x * p1.x + p1.y * p1.y + p1.z * p1.z);
			GLfloat t2 = (p2.x * p2.x + p2.y * p2.y + p2.z * p2.z);
			GLfloat t3 = (p3.x * p3.x + p3.y * p3.y + p3.z * p3.z);
			GLfloat t4 = (p4.x * p4.x + p4.y * p4.y + p4.z * p4.z);

			d[0] = t1; d[1] = p1.y; d[2] = p1.z; d[3] = 1.0;
			d[4] = t2; d[5] = p2.y; d[6] = p2.z; d[7] = 1.0;
			d[8] = t3; d[9] = p3.y; d[10] = p3.z; d[11] = 1.0;
			d[12] = t4; d[13] = p4.y; d[14] = p4.z; d[15] = 1.0;

			e[0] = p1.x; e[1] = t1; e[2] = p1.z; e[3] = 1.0;
			e[4] = p2.x; e[5] = t2; e[6] = p2.z; e[7] = 1.0;
			e[8] = p3.x; e[9] = t3; e[10] = p3.z; e[11] = 1.0;
			e[12] = p4.x; e[13] = t4; e[14] = p4.z; e[15] = 1.0;

			f[0] = p1.x; f[1] = p1.y; f[2] = t1; f[3] = 1.0;
			f[4] = p2.x; f[5] = p2.y; f[6] = t2; f[7] = 1.0;
			f[8] = p3.x; f[9] = p3.y; f[10] = t3; f[11] = 1.0;
			f[12] = p4.x; f[13] = p4.y; f[14] = t4; f[15] = 1.0;

			g[0] = p1.x; g[1] = p1.y; g[2] = p1.z; g[3] = t1;
			g[4] = p2.x; g[5] = p2.y; g[6] = p2.z; g[7] = t2;
			g[8] = p3.x; g[9] = p3.y; g[10] = p3.z; g[11] = t3;
			g[12] = p4.x; g[13] = p4.y; g[14] = p4.z; g[15] = t4;

			glm::mat4 D = glm::make_mat4(d);
			glm::mat4 E = glm::make_mat4(e);
			glm::mat4 F = glm::make_mat4(f);
			glm::mat4 G = glm::make_mat4(g);

			GLfloat detD = glm::determinant(D);
			GLfloat detE = glm::determinant(E);
			GLfloat detF = glm::determinant(F);
			GLfloat detG = glm::determinant(G);

			glm::vec3 center(-detD / 2.0, -detE / 2.0, -detF / 2.0);
			GLfloat radius = (GLfloat) 0.5 * glm::sqrt(glm::pow(detD, 2) + glm::pow(detE, 2) + glm::pow(detE, 2) - (GLfloat) 4.0 * detG);

			this->bs_pos.push_back(center);
			this->bs_radius.push_back(radius);
		}
}
*/

void Object::setPos2D(glm::vec2 v) {

		this->modelview_matrix = glm::translate(glm::mat4(1.0), glm::vec3(v.x, 0.0, v.y));
}

void Object::setPos2D(GLfloat x, GLfloat z){

		this->modelview_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, 0.0, z));
}

void Object::setPos3D(glm::vec3 v){

		this->modelview_matrix = glm::translate(glm::mat4(1.0), v);
}

void Object::setPos3D(GLfloat x, GLfloat y, GLfloat z){

		this->modelview_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
}

void Object::traceCamera(Camera c) {

	GLfloat cam_radius = 15.0;
	GLfloat cam_angle = c.angle + (GLfloat) (90.0 + 70.0);

		glm::vec3 pos = glm::vec3(c.radius * glm::sin(glm::radians(c.angle)), 6, c.radius * glm::cos(glm::radians(c.angle)));
		glm::vec3 move = glm::vec3(cam_radius * glm::sin(glm::radians(cam_angle)), 0, cam_radius * glm::cos(glm::radians(cam_angle)));

		pos = pos + move;

		this->setPos2D(pos.x, pos.z);
		this->rotateObject(cam_angle, glm::vec3(0.0, 1.0, 0.0));
}

void Object::moveObject(glm::vec3 v) {

		this->modelview_matrix = glm::translate(this->modelview_matrix, v);
}

void Object::moveObject(GLfloat x, GLfloat y, GLfloat z) {

		this->modelview_matrix = glm::translate(this->modelview_matrix, glm::vec3(x, y, z));
}

void Object::rotateObject(GLfloat angle, glm::vec3 v) {

		this->modelview_matrix = glm::rotate(this->modelview_matrix, glm::radians(angle), v);
}

void Object::rotateObject(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {

		this->modelview_matrix = glm::rotate(this->modelview_matrix, glm::radians(angle), glm::vec3(x, y, z));
}

void Object::scaleObject(GLfloat ratio, glm::vec3 v) {

		this->modelview_matrix = glm::scale(this->modelview_matrix, ratio * v);
}

void Object::scaleObject(GLfloat ratio, GLfloat x, GLfloat y, GLfloat z) {

		this->modelview_matrix = glm::scale(this->modelview_matrix, glm::vec3(ratio * x, ratio * y, ratio *  z));
}

glm::mat4 Object::modelview() {

	return this->modelview_matrix;
}

void Object::initQueue (int spot) {
		
		// vert[spot] 이 실제 점
		this->renderQueue.push(spot);
}

void Object::updateQueue() {

	int queue_size = this->renderQueue.size();

		for (int i = 0; i < 50000; i++) {

			this->render2[i] = this->render1[i];
		}

		for (int i = 0; i < queue_size; i++) {

			// pop 하기 전의 모든 코드가 인접한 점을 찾는 코드
			int front = this->renderQueue.front();

			for (unsigned int j = 0; j < this->object->faces.size(); j++) {

				int a[] = { 0, 0, 0 };
				int vis = this->object->faces[j].vIndexStart;

				for (int k = 0; k < 3; k++) {

					if (front == this->object->vIndex[vis + k]) {
						
						a[k] = 1;
					}
				}

				if (a[0] == 1 && a[1] == 0 && a[2] == 0) {

					if (render1[this->object->vIndex[vis + 1]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis + 1]);
						render1[this->object->vIndex[vis + 1]] = render_mode;
					}

					if (render1[this->object->vIndex[vis + 2]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis + 2]);
						render1[this->object->vIndex[vis + 2]] = render_mode;
					}
				}
				else if (a[0] == 0 && a[1] == 1 && a[2] == 0) {

					if (render1[this->object->vIndex[vis]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis]);
						render1[this->object->vIndex[vis]] = render_mode;
					}

					if (render1[this->object->vIndex[vis + 2]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis + 2]);
						render1[this->object->vIndex[vis + 2]] = render_mode;
					}
				}
				else if (a[0] == 0 && a[1] == 0 && a[2] == 1) {

					if (render1[this->object->vIndex[vis]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis]);
						render1[this->object->vIndex[vis]] = render_mode;
					}

					if (render1[this->object->vIndex[vis + 1]] != render_mode) {
						
						this->renderQueue.push(this->object->vIndex[vis + 1]);
						render1[this->object->vIndex[vis + 1]] = render_mode;
					}
				}
			}

			this->renderQueue.pop();
		}
}