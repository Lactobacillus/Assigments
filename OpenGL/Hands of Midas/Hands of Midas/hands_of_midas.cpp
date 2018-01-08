/* hands_of_midas.cpp
* Computer Graphics 2017
* Author : Lee Sora
* Author : Cho Youngwoo
* Author : Choi Moonyoung
* 2017.06.12
*/

//#pragma comment(lib,"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.14393.0\\um\\x86\\glew32")
#pragma warning(disable: 4819)

#define RENDER_BASE 0
#define RENDER_ICE 1
#define RENDER_CARTOON 2
#define RENDER_GOLD 3
#define RENDER_METAL 4

#define FOV 0
#define CUBE_SIZE 100.0f
#define DYNAMIC_CUBE_SIZE 10.0f

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx/string_cast.hpp>

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "object.h"
#include "WaveFrontOBJ.h"

int isColideNaive(int target_idx);
glm::vec2 isColideStrict(int target_idx);
void isColidePerfect(int target_idx);

Object cow1("cow.obj");
Object cow2("cow.obj");
Object cow3("cow.obj");
Object cow4("cow.obj");
Object cow5("cow.obj");
Object cow6("cow.obj");
Object cow7("cow.obj");
Object bunny("bunny.obj");
Object hand("hand.obj");

int object_num = 9;

Object * objects[] = {
	&cow1,
	&cow2,
	&cow3,
	&cow4,
	&cow5,
	&cow6,
	&cow7,
	&bunny,
	&hand
};

int camera_idx = 0;

Camera cam[5] = {
	Camera(30, 0),
	Camera(30, 20, 30, 25, 2, 25, 0, 0, -1),
	Camera(20, 10, 40, 25, 2, 25, 0, 1, 0),
	Camera(-28, 18, 28, 0, 2, 0, 0, 1, 0),
	Camera(0, 100, 0, 0, 0, 0, 1, 0, 0)
};

GLfloat normal[] = {

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

GLfloat cubemap_vertices[] = {
	// positions          
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,

	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,

	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,

	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,

	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,

	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE
};

float dynamic_cube_vertices[] = {
	// positions          
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,

	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,

	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,

	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,

	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,

	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE,
	-DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE,
	DYNAMIC_CUBE_SIZE, -DYNAMIC_CUBE_SIZE, DYNAMIC_CUBE_SIZE
};

GLFWwindow * window;

GLfloat cow_degree = 180.0;

unsigned int render_mode = 0;

int main() {

	int frame = 0;

	if (!glfwInit()) {

		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();

		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1280, 720, "Hand of Midas", NULL, NULL);
	if (window == NULL) {

		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();

		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {

		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();

		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window, 640, 360);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Shader Load //////////////////////////////////////////////////////////////////////////////////////////////////////////

	GLuint cubemap_ProgramID = LoadShaders("cube.vert", "cube.frag");
	//GLuint dynamic_ProgramID = LoadShaders("dynamic.vert", "dynamic.frag");
	GLuint cow_ProgramID = LoadShaders("cow.vert", "cow.frag");
	GLuint bunny_ProgramID = LoadShaders("bunny.vert", "bunny.frag");
	GLuint hand_ProgramID = LoadShaders("midas.vert", "midas.frag");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// Cubemap Load /////////////////////////////////////////////////////////////////////////////////////////////////////////

	GLuint cubemap_textureID;
	std::vector <std::string> cubemap_faces;
	glGenTextures(1, &cubemap_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_textureID);
	cubemap_faces.push_back("./skymap/right.jpg");
	cubemap_faces.push_back("./skymap/left.jpg");
	cubemap_faces.push_back("./skymap/top.jpg");
	cubemap_faces.push_back("./skymap/bottom.jpg");
	cubemap_faces.push_back("./skymap/back.jpg");
	cubemap_faces.push_back("./skymap/front.jpg");
	glActiveTexture(GL_TEXTURE0);
	GLuint cubemap_texture = loadCubemap(cubemap_faces);

	GLuint cubemap_VAO;
	GLuint cubemap_pos;
	glGenVertexArrays(1, &cubemap_VAO);
	glGenBuffers(1, &cubemap_pos);
	glBindVertexArray(cubemap_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubemap_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemap_vertices), cubemap_vertices, GL_STATIC_DRAW);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// Dynamic Cubemap Initialize ///////////////////////////////////////////////////////////////////////////////////////////

	GLuint dynamic_framebuffer;
	GLuint dynamic_renderbuffer;
	GLuint dynamic_colorbuffer;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &dynamic_colorbuffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dynamic_colorbuffer);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < 6; i++) {

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &dynamic_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, dynamic_framebuffer);
	glGenRenderbuffers(1, &dynamic_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, dynamic_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamic_colorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dynamic_renderbuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {

		fprintf(stderr, "error in framebuffer");
		getchar();
		glfwTerminate();
	}

	GLuint dynamic_cube_VAO;
	GLuint dynamic_cube_VBO;
	GLuint dynamic_cube_NVBO;
	glGenVertexArrays(1, &dynamic_cube_VAO);
	glGenBuffers(1, &dynamic_cube_VBO);
	glGenBuffers(1, &dynamic_cube_NVBO);
	glBindVertexArray(dynamic_cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, dynamic_cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dynamic_cube_vertices), dynamic_cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, dynamic_cube_NVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);

	std::vector <Camera> dynamic_cam;
	glm::vec3 bunny_center = (bunny.object->bbmax + bunny.object->bbmin) * (GLfloat) 0.5;
	Camera dynamic_cam_right(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x + (GLfloat) 1.0, bunny_center.y, bunny_center.z, 0.0, 1.0, 0.0);
	Camera dynamic_cam_left(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x - (GLfloat) 1.0, bunny_center.y, bunny_center.z, 0.0, 1.0, 0.0);
	Camera dynamic_cam_up(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x, bunny_center.y + (GLfloat) 1.0, bunny_center.z, 0.0, 0.0, -1.0);
	Camera dynamic_cam_bottom(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x, bunny_center.y - (GLfloat) 1.0, bunny_center.z, 0.0, 0.0, 1.0);
	Camera dynamic_cam_back(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x, bunny_center.y, bunny_center.z + (GLfloat) 1.0, 0.0, 1.0, 0.0);
	Camera dynamic_cam_front(bunny_center.x, bunny_center.y, bunny_center.z, bunny_center.x, bunny_center.y, bunny_center.z - (GLfloat) 1.0, 0.0, 1.0, 0.0);

	dynamic_cam.push_back(dynamic_cam_front);
	dynamic_cam.push_back(dynamic_cam_right);
	dynamic_cam.push_back(dynamic_cam_left);
	dynamic_cam.push_back(dynamic_cam_up);
	dynamic_cam.push_back(dynamic_cam_bottom);
	dynamic_cam.push_back(dynamic_cam_back);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// Shader Initialize ////////////////////////////////////////////////////////////////////////////////////////////////////

	GLuint cubemap_MID = glGetUniformLocation(cubemap_ProgramID, "M");
	GLuint cubemap_VID = glGetUniformLocation(cubemap_ProgramID, "V");
	GLuint cubemap_PID = glGetUniformLocation(cubemap_ProgramID, "P");
	GLuint cubemap_TID = glGetUniformLocation(cubemap_ProgramID, "text");

	// test dynamic cube mapping
	//GLuint dynamic_MID = glGetUniformLocation(dynamic_ProgramID, "M");
	//GLuint dynamic_VID = glGetUniformLocation(dynamic_ProgramID, "V");
	//GLuint dynamic_PID = glGetUniformLocation(dynamic_ProgramID, "P");
	//GLuint dynamic_TID = glGetUniformLocation(dynamic_ProgramID, "text");

	GLuint cow_MatrixID = glGetUniformLocation(cow_ProgramID, "MVP");
	GLuint cow_MID = glGetUniformLocation(cow_ProgramID, "M");
	GLuint cow_VID = glGetUniformLocation(cow_ProgramID, "V");
	GLuint cow_PID = glGetUniformLocation(cow_ProgramID, "P");
	GLuint cow_LightID = glGetUniformLocation(cow_ProgramID, "LightPosW");
	GLuint cow_TID = glGetUniformLocation(cow_ProgramID, "text");

	GLuint bunny_MID = glGetUniformLocation(bunny_ProgramID, "M");
	GLuint bunny_VID = glGetUniformLocation(bunny_ProgramID, "V");
	GLuint bunny_PID = glGetUniformLocation(bunny_ProgramID, "P");
	GLuint bunny_TID = glGetUniformLocation(bunny_ProgramID, "text");

	GLuint hand_MatrixID = glGetUniformLocation(hand_ProgramID, "MVP");
	GLuint hand_MID = glGetUniformLocation(hand_ProgramID, "M");
	GLuint hand_VID = glGetUniformLocation(hand_ProgramID, "V");
	GLuint hand_PID = glGetUniformLocation(hand_ProgramID, "P");
	GLuint hand_LightID = glGetUniformLocation(hand_ProgramID, "LightPosW");
	GLuint hand_TID = glGetUniformLocation(hand_ProgramID, "text");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// Object Initialize ////////////////////////////////////////////////////////////////////////////////////////////////////

	
	//cow1.initObject();
	//cow2.initObject();
	//cow3.initObject();
	//cow4.initObject();
	//cow5.initObject();
	//cow6.initObject();
	//cow7.initObject();
	bunny.initObject();
	//hand.initObject();
	
	cow4.setPos3D(25.0, 2.0, 25.0);
	cow5.setPos3D(-25.0, 2.0, 25.0);
	cow6.setPos3D(25.0, 2.0, -25.0);
	cow7.setPos3D(-25.0, 2.0, -25.0);

	cow6.rotateObject(180.0, glm::vec3(0.0, 1.0, 0.0));
	cow7.rotateObject(180.0, glm::vec3(0.0, 1.0, 0.0));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	do {

		frame++;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Camera & Colision Detection //////////////////////////////////////////////////////////////////////////////////////
		
		// colision_naive 와 colision_strict 는 동시에 사용해야 함
		// 만약 colision_perfect 를 사용할 경우에는 두 함수를 모두 꺼야 함
		int colision_naive = isColideNaive(8);
		glm::vec2 colision_strict = isColideStrict(8);

		if (colision_strict.x != -1) {

			objects[(int) colision_strict.x]->initQueue((unsigned int) colision_strict.y);
		}

		if (colision_naive) {

			cam[FOV].w = -cam[FOV].w;
			cam[FOV].v = -cam[FOV].v;
		}
		
		// colision_perfect 만 사용하는 경우
		// 사용하지 못함
		//int colision_naive = 0;
		//isColidePerfect(8);
		
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)      { camera_idx = 0; }
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { camera_idx = 1; }
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { camera_idx = 2; }
		else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { camera_idx = 3; }
		else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { camera_idx = 4; }

		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

			if (colision_naive == 1 || colision_naive == 3) {

				cam[FOV].moveBackward();
			}
			else{

				cam[FOV].moveForward();
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

			if (colision_naive == 1 || colision_naive == 2) {

				cam[FOV].moveRight();
			}
			else{

				cam[FOV].moveLeft();
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

			if (colision_naive == 2 || colision_naive == 4) {

				cam[FOV].moveForward();
			}
			else{

				cam[FOV].moveBackward();
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

			if (colision_naive == 3 || colision_naive == 4) {

				cam[FOV].moveLeft();
			}
			else{

				cam[FOV].moveRight();
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

			cam[FOV].stop();
		}
		else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {

			render_mode = RENDER_BASE;
			hand.initQueue(hand.object->vIndex[0]);
		}
		else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {

			render_mode = RENDER_ICE;
			hand.initQueue(hand.object->vIndex[0]);
		}
		else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {

			render_mode = RENDER_CARTOON;
			hand.initQueue(hand.object->vIndex[0]);
		}
		else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {

			render_mode = RENDER_GOLD;
			hand.initQueue(hand.object->vIndex[0]);
		}
		else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {

			render_mode = RENDER_METAL;
			hand.initQueue(hand.object->vIndex[0]);
		}
		else {

		}

		for (int i = 0; i < sizeof(cam) / sizeof(Camera); i++) {

			cam[i].refresh();
		}

		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 300.0f);
		glm::mat4 ViewMatrix = cam[camera_idx].lookAt();
		glm::mat4 world_ModelMatrix = glm::mat4(1.0);
		glm::mat4 world_MVP = ProjectionMatrix * ViewMatrix * world_ModelMatrix;

		cow1.updateQueue();
		cow2.updateQueue();
		cow3.updateQueue();
		cow4.updateQueue();
		cow5.updateQueue();
		cow6.updateQueue();
		cow7.updateQueue();
		hand.updateQueue();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		// Lighting /////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 움직이는 조명
		GLfloat light1_x = (GLfloat) 10.0 * glm::sin(glm::radians(cow_degree * (GLfloat) 4.0));
		GLfloat light1_y = (GLfloat) 10.0 + (GLfloat) 5.0 * glm::sin(glm::radians(cow_degree * (GLfloat) -2.0));
		GLfloat light1_z = (GLfloat) 15.0 * glm::cos(glm::radians(cow_degree * (GLfloat) 4.0));
		glm::vec4 light1 = glm::vec4(light1_x, light1_y, light1_z, 1.0);

		// 움직이지 않는 조명
		//glm::vec4 light1 = glm::vec4(0.0, 10.0, 0.0, 1.0);
		
		glm::mat4 light1_ModelMatrix(1.0);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		// Object Positioning & Tracing Camera //////////////////////////////////////////////////////////////////////////////

		cow1.setPos3D(glm::vec3(15.0 * glm::sin(glm::radians(cow_degree)), 2.0, 15.0 * glm::cos(glm::radians(cow_degree))));
		cow2.setPos3D(glm::vec3(0.0, 20.0 * glm::sin(glm::radians(-cow_degree)), 20.0 * glm::cos(glm::radians(-cow_degree))));
		cow3.setPos3D(glm::vec3(25.0 * glm::sin(glm::radians(cow_degree)), 25.0 * glm::cos(glm::radians(cow_degree)), 0.0));
		cow1.rotateObject((GLfloat) -2.0 * cow_degree, glm::vec3(0.0, 1.0, 0.0));
		cow2.rotateObject(-cow_degree, glm::vec3(0.0, 1.0, 0.0));
		cow3.rotateObject(cow_degree, glm::vec3(0.0, 1.0, 0.0));
		cow_degree += (GLfloat) 0.5;
		
		glm::mat4 cow1_ModelMatrix = cow1.modelview();
		glm::mat4 cow1_MVP = ProjectionMatrix * ViewMatrix * cow1_ModelMatrix;
		glm::mat4 cow2_ModelMatrix = cow2.modelview();
		glm::mat4 cow2_MVP = ProjectionMatrix * ViewMatrix * cow2_ModelMatrix;
		glm::mat4 cow3_ModelMatrix = cow3.modelview();
		glm::mat4 cow3_MVP = ProjectionMatrix * ViewMatrix * cow3_ModelMatrix;
		glm::mat4 cow4_ModelMatrix = cow4.modelview();
		glm::mat4 cow4_MVP = ProjectionMatrix * ViewMatrix * cow4_ModelMatrix;
		glm::mat4 cow5_ModelMatrix = cow5.modelview();
		glm::mat4 cow5_MVP = ProjectionMatrix * ViewMatrix * cow5_ModelMatrix;
		glm::mat4 cow6_ModelMatrix = cow6.modelview();
		glm::mat4 cow6_MVP = ProjectionMatrix * ViewMatrix * cow6_ModelMatrix;
		glm::mat4 cow7_ModelMatrix = cow7.modelview();
		glm::mat4 cow7_MVP = ProjectionMatrix * ViewMatrix * cow7_ModelMatrix;

		glm::mat4 dynamic_ModelMatrix = glm::mat4(1.0);
		glm::mat4 bunny_ModelMatrix = bunny.modelview();

		hand.traceCamera(cam[0]);
		glm::mat4 hand_ModelMatrix = hand.modelview();
		glm::mat4 hand_MVP = ProjectionMatrix * ViewMatrix * hand_ModelMatrix;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		
		// Dynamic Environment Mapping //////////////////////////////////////////////////////////////////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, dynamic_framebuffer);
		glViewport(0, 0, 1024, 1024);

		for (unsigned int i = 0; i < dynamic_cam.size(); i++) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glm::mat4 dynamic_ViewMatrix = glm::mat4(glm::mat3(dynamic_cam[i].lookAt()));
			glm::mat4 dynamic_ProjectionMatrix = glm::perspective(glm::radians(90.0), 1.0, 5.0, 110.0);
			dynamic_ProjectionMatrix = glm::scale(dynamic_ProjectionMatrix, glm::vec3(-1.0, -1.0, 1.0));


			glDepthMask(GL_FALSE);
			glUseProgram(cubemap_ProgramID);
			glBindVertexArray(cubemap_VAO);
			glUniformMatrix4fv(cubemap_MID, 1, GL_FALSE, &world_ModelMatrix[0][0]);
			glUniformMatrix4fv(cubemap_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cubemap_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubemap_pos);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthMask(GL_TRUE);

			cow1.initObject();
			glm::mat4 dynamic_cow1_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow1_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow1_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow1_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow1.drawObject1();
			cow1.drawObject2();
			cow2.clearObject();

			cow2.initObject();
			glm::mat4 dynamic_cow2_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow2_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow2_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow2_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow2.drawObject1();
			cow2.drawObject2();
			cow2.clearObject();

			cow3.initObject();
			glm::mat4 dynamic_cow3_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow3_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow3_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow3_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow3.drawObject1();
			cow3.drawObject2();
			cow3.clearObject();

			cow4.initObject();
			glm::mat4 dynamic_cow4_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow4_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow4_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow4_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow4.drawObject1();
			cow4.drawObject2();
			cow4.clearObject();

			cow5.initObject();
			glm::mat4 dynamic_cow5_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow5_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow5_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow5_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow5.drawObject1();
			cow5.drawObject2();
			cow5.clearObject();

			cow6.initObject();
			glm::mat4 dynamic_cow6_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow6_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow6_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow6_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow6.drawObject1();
			cow6.drawObject2();
			cow6.clearObject();

			cow7.initObject();
			glm::mat4 dynamic_cow7_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * cow7_ModelMatrix;
			glUseProgram(cow_ProgramID);
			glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_cow7_MVP[0][0]);
			glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow7_ModelMatrix[0][0]);
			glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			cow7.drawObject1();
			cow7.drawObject2();
			cow7.clearObject();

			hand.initObject();
			glm::mat4 dynamic_hand_MVP = dynamic_ProjectionMatrix * dynamic_ViewMatrix * hand_ModelMatrix;
			glUseProgram(hand_ProgramID);
			glUniform3f(hand_LightID, light1.x, light1.y, light1.z);
			glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &dynamic_hand_MVP[0][0]);
			glUniformMatrix4fv(hand_MID, 1, GL_FALSE, &hand_ModelMatrix[0][0]);
			glUniformMatrix4fv(hand_VID, 1, GL_FALSE, &dynamic_ViewMatrix[0][0]);
			glUniformMatrix4fv(hand_PID, 1, GL_FALSE, &dynamic_ProjectionMatrix[0][0]);
			glUniform1i(cubemap_TID, 0);
			hand.drawObject1();
			hand.drawObject2();
			hand.clearObject();

			glBindTexture(GL_TEXTURE_CUBE_MAP, dynamic_colorbuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dynamic_colorbuffer, 0);
			glClearColor(0.0, 0.0, 0.0, 1.0);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 1280, 720);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		


		// Shading Objects //////////////////////////////////////////////////////////////////////////////////////////////////

		glDepthMask(GL_FALSE);
		glUseProgram(cubemap_ProgramID);
		glBindVertexArray(cubemap_VAO);
		glUniformMatrix4fv(cubemap_MID, 1, GL_FALSE, &world_ModelMatrix[0][0]);
		glUniformMatrix4fv(cubemap_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cubemap_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubemap_pos);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glDisableVertexAttribArray(0);

		glUseProgram(bunny_ProgramID);
		glUniformMatrix4fv(bunny_MID, 1, GL_FALSE, &bunny_ModelMatrix[0][0]);
		glUniformMatrix4fv(bunny_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(bunny_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(bunny_TID, 1);
		bunny.drawObject1();
		glActiveTexture(GL_TEXTURE1);
		glBindBuffer(GL_TEXTURE_CUBE_MAP, dynamic_colorbuffer);
		bunny.drawObject2();

		cow1.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow1_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow1_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow1.drawObject1();
		cow1.drawObject2();
		cow1.updateQueue();
		cow1.clearObject();

		cow2.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow2_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow2_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow2.drawObject1();
		cow2.drawObject2();
		cow2.updateQueue();
		cow2.clearObject();

		cow3.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow3_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow3_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow3.drawObject1();
		cow3.drawObject2();
		cow3.updateQueue();
		cow3.clearObject();

		cow4.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow4_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow4_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow4.drawObject1();
		cow4.drawObject2();
		cow4.updateQueue();
		cow4.clearObject();

		cow5.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow5_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow5_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow5.drawObject1();
		cow5.drawObject2();
		cow5.updateQueue();
		cow5.clearObject();

		cow6.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow6_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow6_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow6.drawObject1();
		cow6.drawObject2();
		cow6.updateQueue();
		cow6.clearObject();

		cow7.initObject();
		glUseProgram(cow_ProgramID);
		glUniform3f(cow_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(cow_MatrixID, 1, GL_FALSE, &cow7_MVP[0][0]);
		glUniformMatrix4fv(cow_MID, 1, GL_FALSE, &cow7_ModelMatrix[0][0]);
		glUniformMatrix4fv(cow_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(cow_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		cow7.drawObject1();
		cow7.drawObject2();
		cow7.updateQueue();
		cow7.clearObject();

		hand.initObject();
		glUseProgram(hand_ProgramID);
		glUniform3f(hand_LightID, light1.x, light1.y, light1.z);
		glUniformMatrix4fv(hand_MatrixID, 1, GL_FALSE, &hand_MVP[0][0]);
		glUniformMatrix4fv(hand_MID, 1, GL_FALSE, &hand_ModelMatrix[0][0]);
		glUniformMatrix4fv(hand_VID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(hand_PID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniform1i(cubemap_TID, 0);
		hand.drawObject1();
		hand.drawObject2();
		hand.updateQueue();
		hand.clearObject();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		glfwPollEvents();
		glfwSwapBuffers(window);

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);


	// Clear Buffers ////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	cow1.clearObject();
	cow2.clearObject();
	cow3.clearObject();
	cow4.clearObject();
	cow5.clearObject();
	cow6.clearObject();
	cow7.clearObject();
	bunny.clearObject();
	hand.clearObject();
	*/
	glDeleteProgram(cubemap_ProgramID);
	//glDeleteProgram(dynamic_ProgramID);
	glDeleteProgram(cow_ProgramID);
	glDeleteProgram(bunny_ProgramID);
	glDeleteProgram(hand_ProgramID);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glfwTerminate();

	return 0;
}

int isColideNaive(int target_idx) {

	Object * target = objects[target_idx];
	glm::mat4 target_mv = target->modelview();
	glm::vec3 target_bbmax = glm::vec3(target_mv * glm::vec4(target->object->bbmax, 1));
	glm::vec3 target_bbmin = glm::vec3(target_mv * glm::vec4(target->object->bbmin, 1));

		for (int i = 0; i < object_num; i++) {

			if (i != target_idx) {

				Object * temp = objects[i];
				glm::mat4 temp_mv = temp->modelview();
				glm::vec3 temp_bbmax = glm::vec3(temp_mv * glm::vec4(temp->object->bbmax, 1));
				glm::vec3 temp_bbmin = glm::vec3(temp_mv * glm::vec4(temp->object->bbmin, 1));
			
				// 시계방향으로 벡터를 돌리면서 convexity 를 보려면 temp_bbmax 가 temp_bbmin 보다 커야 함
				// 물체가 (0.0, 1.0, 0.0) 축으로 회전하는 경우, temp_bbmax 와 temp_bbmin 이 뒤바뀔 수 있음
				// 이런 경우에는 모든 면 뒤에 있는 것이 아닌, 모든 면 앞에 있는 걸 충돌로 판정해야 함
				// 위의 if - else if 문들은 모든 면 뒤에 있는 걸을 찾고
				// 아래의 if - else if 문들은 모든 면 앞에 있는 걸 찾음

				glm::vec3 up(0.0, 1.0, 0.0);
				glm::vec3 v1(temp_bbmax.x, 0.0, temp_bbmin.z);
				glm::vec3 v2(temp_bbmin.x, 0.0, temp_bbmin.z);
				glm::vec3 v3(temp_bbmin.x, 0.0, temp_bbmax.z);
				glm::vec3 v4(temp_bbmax.x, 0.0, temp_bbmax.z);
				glm::vec3 v5(target_bbmax.x, 0.0, target_bbmin.z);
				glm::vec3 v6(target_bbmin.x, 0.0, target_bbmin.z);
				glm::vec3 v7(target_bbmin.x, 0.0, target_bbmax.z);
				glm::vec3 v8(target_bbmax.x, 0.0, target_bbmax.z);
				glm::vec3 f1 = glm::normalize(glm::cross(v2 - v1, up));
				glm::vec3 f2 = glm::normalize(glm::cross(v3 - v2, up));
				glm::vec3 f3 = glm::normalize(glm::cross(v4 - v3, up));
				glm::vec3 f4 = glm::normalize(glm::cross(v1 - v4, up));
				glm::vec3 f5 = glm::normalize(glm::cross(v6 - v5, up));
				glm::vec3 f6 = glm::normalize(glm::cross(v7 - v6, up));
				glm::vec3 f7 = glm::normalize(glm::cross(v8 - v7, up));
				glm::vec3 f8 = glm::normalize(glm::cross(v5 - v8, up));
				glm::vec3 target_lu(target_bbmax.x, 0.0, target_bbmax.z);
				glm::vec3 target_ld(target_bbmax.x, 0.0, target_bbmin.z);
				glm::vec3 target_ru(target_bbmin.x, 0.0, target_bbmax.z);
				glm::vec3 target_rd(target_bbmin.x, 0.0, target_bbmin.z);
				glm::vec3 temp_lu(temp_bbmax.x, 0.0, temp_bbmax.z);
				glm::vec3 temp_ld(temp_bbmax.x, 0.0, temp_bbmin.z);
				glm::vec3 temp_ru(temp_bbmin.x, 0.0, temp_bbmax.z);
				glm::vec3 temp_rd(temp_bbmin.x, 0.0, temp_bbmin.z);
				GLfloat d1 = glm::dot(f1, v1);
				GLfloat d2 = glm::dot(f2, v2);
				GLfloat d3 = glm::dot(f3, v3);
				GLfloat d4 = glm::dot(f4, v4);
				GLfloat d5 = glm::dot(f5, v5);
				GLfloat d6 = glm::dot(f6, v6);
				GLfloat d7 = glm::dot(f7, v7);
				GLfloat d8 = glm::dot(f8, v8);

				if (glm::dot(f1, target_lu) - d1 < 0 && glm::dot(f2, target_lu) - d2 < 0 && glm::dot(f3, target_lu) - d3 < 0 && glm::dot(f4, target_lu) - d4 < 0) {

					// 손의 왼쪽 위에서 충돌, w 와 a 키로 이동 불가
					return 1;
				}
				else if (glm::dot(f1, target_ld) - d1 < 0 && glm::dot(f2, target_ld) - d2 < 0 && glm::dot(f3, target_ld) - d3 < 0 && glm::dot(f4, target_ld) - d4 < 0) {

					// 손의 왼쪽 아래에서 충돌, s 와 a 키로 이동 불가
					return 2;
				}
				else if (glm::dot(f1, target_ru) - d1 < 0 && glm::dot(f2, target_ru) - d2 < 0 && glm::dot(f3, target_ru) - d3 < 0 && glm::dot(f4, target_ru) - d4 < 0) {

					// 손의 오른쪽 위에서 충돌, w 와 d 키로 이동 불가
					return 3;
				}
				else if (glm::dot(f1, target_rd) - d1 < 0 && glm::dot(f2, target_rd) - d2 < 0 && glm::dot(f3, target_rd) - d3 < 0 && glm::dot(f4, target_rd) - d4 < 0) {

					// 손의 오른쪽 아래에서 충돌, s 키와 d 키로 이동 불가
					return 4;
				}
				else if (glm::dot(f5, temp_lu) - d5 < 0 && glm::dot(f6, temp_lu) - d6 < 0 && glm::dot(f7, temp_lu) - d7 < 0 && glm::dot(f8, temp_lu) - d8 < 0) {

					// 타겟의 왼쪽 위에서 충돌, s 와 d 키로 이동 불가
					return 4;
				}
				else if (glm::dot(f5, temp_ld) - d5 < 0 && glm::dot(f6, temp_ld) - d6 < 0 && glm::dot(f7, temp_ld) - d7 < 0 && glm::dot(f8, temp_ld) - d8 < 0) {

					// 타겟의 왼쪽 아래에서 충돌, w 와 d 키로 이동 불가
					return 3;
				}
				else if (glm::dot(f5, temp_ru) - d5 < 0 && glm::dot(f6, temp_ru) - d6 < 0 && glm::dot(f7, temp_ru) - d7 < 0 && glm::dot(f8, temp_ru) - d8 < 0) {

					// 타겟의 오른쪽 위에서 충돌, s 와 a 키로 이동 불가
					return 2;
				}
				else if (glm::dot(f5, temp_rd) - d5 < 0 && glm::dot(f6, temp_rd) - d6 < 0 && glm::dot(f7, temp_rd) - d7 < 0 && glm::dot(f8, temp_rd) - d8 < 0) {

					// 타겟의 오른쪽 아래에서 충돌, w 키와 a 키로 이동 불가
					return 1;
				}


				if (glm::dot(f1, target_lu) - d1 > 0 && glm::dot(f2, target_lu) - d2 > 0 && glm::dot(f3, target_lu) - d3 > 0 && glm::dot(f4, target_lu) - d4 > 0) {

					// 손의 왼쪽 위에서 충돌, w 와 a 키로 이동 불가
					return 1;
				}
				else if (glm::dot(f1, target_ld) - d1 > 0 && glm::dot(f2, target_ld) - d2 > 0 && glm::dot(f3, target_ld) - d3 > 0 && glm::dot(f4, target_ld) - d4 > 0) {

					// 손의 왼쪽 아래에서 충돌, s 와 a 키로 이동 불가
					return 3;
				}
				else if (glm::dot(f1, target_ru) - d1 > 0 && glm::dot(f2, target_ru) - d2 > 0 && glm::dot(f3, target_ru) - d3 > 0 && glm::dot(f4, target_ru) - d4 > 0) {

					// 손의 오른쪽 위에서 충돌, w 와 d 키로 이동 불가
					return 2;
				}
				else if (glm::dot(f1, target_rd) - d1 > 0 && glm::dot(f2, target_rd) - d2 > 0 && glm::dot(f3, target_rd) - d3 > 0 && glm::dot(f4, target_rd) - d4 > 0) {

					// 손의 오른쪽 아래에서 충돌, s 키와 d 키로 이동 불가
					return 4;
				}
				else if (glm::dot(f5, temp_lu) - d5 > 0 && glm::dot(f6, temp_lu) - d6 > 0 && glm::dot(f7, temp_lu) - d7 > 0 && glm::dot(f8, temp_lu) - d8 > 0) {

					// 타겟의 왼쪽 위에서 충돌, s 와 d 키로 이동 불가
					return 4;
				}
				else if (glm::dot(f5, temp_ld) - d5 > 0 && glm::dot(f6, temp_ld) - d6 > 0 && glm::dot(f7, temp_ld) - d7 > 0 && glm::dot(f8, temp_ld) - d8 > 0) {

					// 타겟의 왼쪽 아래에서 충돌, w 와 d 키로 이동 불가
					return 2;
				}
				else if (glm::dot(f5, temp_ru) - d5 > 0 && glm::dot(f6, temp_ru) - d6 > 0 && glm::dot(f7, temp_ru) - d7 > 0 && glm::dot(f8, temp_ru) - d8 > 0) {

					// 타겟의 오른쪽 위에서 충돌, s 와 a 키로 이동 불가
					return 3;
				}
				else if (glm::dot(f5, temp_rd) - d5 > 0 && glm::dot(f6, temp_rd) - d6 > 0 && glm::dot(f7, temp_rd) - d7 > 0 && glm::dot(f8, temp_rd) - d8 > 0) {

					// 타겟의 오른쪽 아래에서 충돌, w 키와 a 키로 이동 불가
					return 1;
				}
			}
		}

	return 0;
}

glm::vec2 isColideStrict(int target_idx) {

	int near_idx = 0;
	GLfloat near_distance = 10000.0;

	Object * target = objects[target_idx];
	glm::mat4 target_mv = target->modelview();
	GLfloat * target_mv1D = glm::value_ptr(target_mv);

		for (int i = 0; i < object_num; i++) {

			if (i != target_idx) {

				Object * temp = objects[i];
				glm::mat4 temp_mv = temp->modelview();
				GLfloat * temp_mv1D = glm::value_ptr(temp_mv);

				GLfloat distance = glm::distance(glm::vec3(target_mv1D[12], target_mv1D[13], target_mv1D[14]), glm::vec3(temp_mv1D[12], temp_mv1D[13], temp_mv1D[14]));

				if (near_distance > distance) {

					near_idx = i;
					near_distance = distance;
				}
			}
		}
		
		near_distance = 10000.0;
		int near_vertex_idx = 0;
		glm::mat4 temp_mv = objects[near_idx]->modelview();
		glm::vec3 bbmax = glm::vec3(target_mv * glm::vec4(target->object->bbmax, 1));
		glm::vec3 bbmin = glm::vec3(target_mv * glm::vec4(target->object->bbmin, 1));
		glm::vec3 touch_point = glm::vec3((bbmax.x + bbmin.x) / 2.0, (bbmax.y + bbmin.y) / 2.0, bbmax.z);

		for (unsigned int i = 0; i < objects[near_idx]->object->vIndex.size(); i++) {

			glm::vec3 pos = glm::vec3(temp_mv * glm::vec4(objects[near_idx]->object->verts[objects[near_idx]->object->vIndex[i]], 1));
			GLfloat distance = glm::distance(pos, touch_point);

			if (near_distance > distance) {

				near_vertex_idx = i;
				near_distance = distance;
			}
		}

		if (near_distance < (GLfloat) 3.2) {

			return glm::vec2(near_idx, objects[near_idx]->object->vIndex[near_vertex_idx]);
		}

	return glm::vec2(-1, -1);
}
/*
void isColidePerfect(int target_idx) {

	// vertex 사이의 간격이 너무 좁아서 실패

	int near_object_idx = 0;
	GLfloat near_object_distance = 10000.0;

	Object * target = objects[target_idx];
	glm::mat4 target_mv = target->modelview();
	GLfloat * target_mv1D = glm::value_ptr(target_mv);

		// 가장 가까운 오브젝트를 찾음
		for (int i = 0; i < object_num; i++) {

			if (i != target_idx) {

				Object * temp = objects[i];
				glm::mat4 temp_mv = temp->modelview();
				GLfloat * temp_mv1D = glm::value_ptr(temp_mv);

				GLfloat distance = glm::distance(glm::vec3(target_mv1D[12], target_mv1D[13], target_mv1D[14]), glm::vec3(temp_mv1D[12], temp_mv1D[13], temp_mv1D[14]));

				if (near_object_distance > distance) {

					near_object_idx = i;
					near_object_distance = distance;
				}
			}
		}

		int near_vertex_idx = 0;
		GLfloat near_vertex_distance = 10000.0;
		glm::mat4 temp_mv = objects[near_object_idx]->modelview();
		glm::vec3 bbmax = glm::vec3(target_mv * glm::vec4(target->object->bbmax, 1));
		glm::vec3 bbmin = glm::vec3(target_mv * glm::vec4(target->object->bbmin, 1));
		glm::vec3 touch_point = glm::vec3((bbmax.x + bbmin.x) / 2.0, (bbmax.y + bbmin.y) / 2.0, bbmax.z);
		
		// 가장 가까운 오브젝트에서 손의 touch_point 와 가장 가까운 target_vertex 를 찾음
		for (unsigned int i = 0; i < objects[near_object_idx]->object->vIndex.size(); i++) {

			glm::vec3 pos = glm::vec3(temp_mv * glm::vec4(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[i]], 1));
			GLfloat distance = glm::distance(pos, touch_point);

			if (near_vertex_distance > distance) {

				near_vertex_distance = distance;
				near_vertex_idx = i;
			}
		}

		std::vector <glm::vec3> near_vertices;
		std::vector <Face> near_faces;
		glm::vec3 target_v = objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[near_vertex_idx]];

		// target vertex 와 바로 인접한 vertex와 target vertex 가 들어 있는 faces 를 찾음
		for (unsigned int i = 0; i < objects[near_object_idx]->object->faces.size(); i++) {

			int vi = objects[near_object_idx]->object->faces[i].vIndexStart;

			if (objects[near_object_idx]->object->vIndex[near_vertex_idx] == objects[near_object_idx]->object->vIndex[vi]) {

				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi + 1]]);
				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi + 2]]);
				near_faces.push_back(objects[near_object_idx]->object->faces[i]);
			}
			else if (objects[near_object_idx]->object->vIndex[near_vertex_idx] == objects[near_object_idx]->object->vIndex[vi + 1]) {

				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi]]);
				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi + 2]]);
				near_faces.push_back(objects[near_object_idx]->object->faces[i]);
			}
			else if (objects[near_object_idx]->object->vIndex[near_vertex_idx] == objects[near_object_idx]->object->vIndex[vi + 2]) {

				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi]]);
				near_vertices.push_back(objects[near_object_idx]->object->verts[objects[near_object_idx]->object->vIndex[vi + 1]]);
				near_faces.push_back(objects[near_object_idx]->object->faces[i]);
			}
		}

		// 바로 인접한 vertex 3 개를 뽑아 면을 생성하고, target vertex 와의 거리를 비교해서 convex 인지 concave 인지를 판별
		glm::vec3 v1;
		glm::vec3 v2;
		glm::vec3 v3;
		int check = 1;
		srand((unsigned int) time(NULL));
		target_v = glm::vec3(temp_mv * glm::vec4(target_v, 1));
	
		do {

			v1 = near_vertices[rand() % near_vertices.size() + 1];
			v2 = near_vertices[rand() % near_vertices.size() + 1];
			v3 = near_vertices[rand() % near_vertices.size() + 1];

		} while (glm::distance(v1, v2) > 0 && glm::distance(v2, v3) > 0 && glm::distance(v3, v1) > 0);

		if (glm::dot(glm::normalize(glm::cross(v2 - v1, v3 - v2)), target_v) - glm::dot(glm::normalize(glm::cross(v2 - v1, v3 - v2)), v1) > 0) {

			for (unsigned int j = 0; j < near_faces.size(); j++) {

				glm::vec3 normal = glm::normalize(glm::vec3(temp_mv * glm::vec4(near_faces[j].normal, 1.0)));
				GLfloat projection = glm::dot(normal, touch_point) - glm::dot(normal, target_v);

				if (projection > 0) {

					check = 0;
				}
			}
		}
		else {

			for (unsigned int j = 0; j < near_faces.size(); j++) {

				glm::vec3 normal = glm::normalize(glm::vec3(temp_mv * glm::vec4(near_faces[j].normal, 1.0)));
				GLfloat projection = glm::dot(normal, touch_point) - glm::dot(normal, target_v);

				if (projection < 0) {

					check = 0;
				}
			}
		}

		if (check) {

			objects[near_object_idx]->initQueue(near_vertex_idx);
		}
		else{

		}
}
*/