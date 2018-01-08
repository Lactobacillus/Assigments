#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H 

#include <stdlib.h>
#include <math.h>
#include <vector>

#include <glm\glm.hpp>

class Face {

public:

	int vCount;
	int vIndexStart;
	glm::vec3 normal;

	Face (int vIndexStart = -1) : vIndexStart(vIndexStart), vCount(0) {}
};


class WaveFrontOBJ  {

public:

	glm::vec3 bbmax;
	glm::vec3 bbmin;
	std::vector <glm::vec3> verts;
	std::vector <glm::vec3> normals;
	std::vector <glm::vec2> texCoords;
	std::vector <glm::vec3> faceNormals;
	std::vector <Face> faces;
	std::vector <int> vIndex;
	std::vector <int> nIndex;
	std::vector <int> tIndex;

	bool isFlat;
	GLuint mode;

	// Vertex bbmin
	// Vertex bbmax;
	// bounding box

	WaveFrontOBJ(char * filename);
	void Draw();

protected:

	glm::vec3 faceNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);
	void vertexNormal();
	void computeBoundingBox();
};


#endif