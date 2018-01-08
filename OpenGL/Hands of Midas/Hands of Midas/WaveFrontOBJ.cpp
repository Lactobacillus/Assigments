#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>

#include <GL\glut.h>
#include <glm\glm.hpp>

#include "WaveFrontOBJ.h"

using namespace std;

WaveFrontOBJ::WaveFrontOBJ (char *filename) {
	
	isFlat = true;
	mode = GL_POLYGON;

	char *line = new char[200];
	char *line_back = new char[200];
	char wspace[] = " \t\n";
	char separator[] = "/";
	char *token;
	int indices[3];
	float x, y, z;
	float tex_u, tex_v;

	ifstream file(filename);
	if (!file) {
		
		cerr << "Cannot open file: " << filename << " exiting." << endl;
		exit(-1);
	}

	while (!file.eof()) {
		file.getline(line, 199);
		// first, strip off comments
		if (line[0] == '#')
			continue;
		else if (!strcmp(line, ""))
			continue;
		else {
			strcpy(line_back, line); // strtok destroys line.

			token = strtok(line, wspace);

			if (!strcmp(token, "v")) {
				x = atof(strtok(NULL, wspace));
				y = atof(strtok(NULL, wspace));
				z = atof(strtok(NULL, wspace));
				verts.push_back(glm::vec3(x, y, z));
			}
			else if (!strcmp(token, "vn")) {
				x = atof(strtok(NULL, wspace));
				y = atof(strtok(NULL, wspace));
				z = atof(strtok(NULL, wspace));
				normals.push_back(normalize(glm::vec3(x, y, z)));
			}
			else if (!strcmp(token, "vt")) {
				tex_u = atof(strtok(NULL, wspace));
				tex_v = atof(strtok(NULL, wspace));
				texCoords.push_back(glm::vec2(tex_u, tex_v));
			}
			else if (!strcmp(token, "f")) {
				int vi = (int)vIndex.size();
				faces.push_back(Face(vi));
				Face& curFace = faces.back();
				for (char *p = strtok(NULL, wspace); p; p = strtok(NULL, wspace)) {
					indices[0] = 0;
					indices[1] = 0;
					indices[2] = 0;
					char* pos = p;
					int len = (int)strlen(p);
					for (int j = 0, i = 0; j <= len && i < 3; j++) {
						if (p[j] == '/' || p[j] == 0) {
							p[j] = 0;
							indices[i++] = atoi(pos);
							pos = p + j + 1;
						}
					}

					vIndex.push_back(indices[0] - 1);
					tIndex.push_back(indices[1] - 1);
					nIndex.push_back(indices[2] - 1);
					curFace.vCount++;

					if (indices[2] != 0)
						isFlat = false;
				}
				if (curFace.vCount > 2){
					curFace.normal = faceNormal(verts[vIndex[vi]], verts[vIndex[vi + 1]], verts[vIndex[vi + 2]]);
					curFace.normal = normalize(curFace.normal);
					faceNormals.push_back(curFace.normal);
				}
			}


			else if (!strcmp(token, "g")) {      // group
			}
			else if (!strcmp(token, "s")) {      // smoothing group
			}
			else if (!strcmp(token, "u")) {      // material line
			}
			else if (!strcmp(token, "")) {       // blank line
			}
			else {
				cout << line_back << endl;
			}
		}
	}
	vertexNormal();
	computeBoundingBox();
}


//------------------------------------------------------------------------------
// face normal 을 계산
// Compute face normal
glm::vec3 WaveFrontOBJ::faceNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
	
	return glm::cross(v1 - v0, v2 - v0);
}

//-----------------------------------------------------------------------------
// OpenGL API 를 사용해서 파일에서 읽어둔 object 를 그린다.
// Draw object which is read from file
void WaveFrontOBJ::Draw() {
	int i;

	for (int f = 0; f < (int)faces.size(); f++) {
		Face& curFace = faces[f];
		glBegin(mode);
		for (int v = 0; v < curFace.vCount; v++) {
			int vi = curFace.vIndexStart + v;

			if (isFlat) {
				if (v == 0) {
					glNormal3f(curFace.normal.x, curFace.normal.y, curFace.normal.z);
				}
			}

			else if ((i = nIndex[vi]) >= 0) {
				glNormal3f(normals[i].x, normals[i].y, normals[i].z);
			}

			if ((i = tIndex[vi]) >= 0) {
				glTexCoord2f(texCoords[i].x, texCoords[i].y);
			}
			if ((i = vIndex[vi]) >= 0) {
				glVertex3f(verts[i].x, verts[i].y, verts[i].z);
			}
		}
		glEnd();
	}
}

void WaveFrontOBJ::vertexNormal(){

	// PA #4 의 수정된 부분 #8
	// vertex normal 을 계산한다.
	// 모든 vertex 에 대해서 인접한 face 를 찾아 그것들의 average face normal 을 계산한다.
	// 첫 번째 for 문에서 vertex 를 하나 고른다.
	// 두 번째 for 문과 세 번째 for 문에서 그 vertex 와 인접한 face 를 찾아 face normal 을 더해준다.
	// 더한 face normal 을 normalize 하면 average face normal == vertex normal 이 된다.
	// (face normal 들이 원래 normalize 되었기 때문에 평균값과 일치한다.)
	for (int i = 0; i < (int)verts.size(); i++){

		glm::vec3 temp(0.0, 0.0, 0.0);

		for (int j = 0; j < (int)faces.size(); j++){
			for (int k = faces[j].vIndexStart; k < faces[j].vIndexStart + faces[j].vCount; k++){

				if (vIndex[k] == i){

					temp = temp + faces[j].normal;
				}
			}
		}

		temp = glm::normalize(temp);
		normals.push_back(temp);
	}
}

void WaveFrontOBJ::computeBoundingBox () {
	
	GLfloat bbmin_x = verts[0].x;
	GLfloat bbmin_y = verts[0].y;
	GLfloat bbmin_z = verts[0].z;
	GLfloat bbmax_x = verts[0].x;
	GLfloat bbmax_y = verts[0].y;
	GLfloat bbmax_z = verts[0].z;

		if (verts.size() > 0) {
		
			for(unsigned int i=1; i < (int)verts.size(); i++){

				if (verts[i].x < bbmin_x) {

					bbmin_x = verts[i].x;
				}
				if (verts[i].y < bbmin_y) {

					bbmin_y = verts[i].y;
				}
				if (verts[i].z < bbmin_z) {
				
					bbmin_z = verts[i].z;
				}
				if (verts[i].x > bbmax_x) {

					bbmax_x = verts[i].x;
				}
				if (verts[i].y > bbmax_y) {

					bbmax_y = verts[i].y;
				}
				if (verts[i].z > bbmax_z) {

					bbmax_z = verts[i].z;
				}
			}

			this->bbmax = glm::vec3(bbmax_x, bbmax_y, bbmax_z);
			this->bbmin = glm::vec3(bbmin_x, bbmin_y, bbmin_z);
		}
}