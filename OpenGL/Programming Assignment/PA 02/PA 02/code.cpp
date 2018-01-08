/*
*  Programming Assignment #2
*  Computer Graphics 2017
*  Author : Cho Youngwoo
*  2017.04.04
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <GL/glut.h>
#include "Matrix.h"
#include "WaveFrontOBJ.h"

// 'cameras' stores infomation of 5 cameras.
float cameras[5][9] =
{
	{ 28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ 28, 18, -28, 0, 2, 0, 0, 1, 0 },
	{ -28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ -12, 12, 0, 0, 2, 0, 0, 1, 0 },
	{ 0, 100, 0, 0, 0, 0, 1, 0, 0 }
};
int cameraCount = sizeof(cameras) / sizeof(cameras[0]);

int cameraIndex;
vector<Matrix> wld2cam, cam2wld;

// Variables for 'cow' object.
Matrix cow2wld;
WaveFrontOBJ* cow;

unsigned floorTexID;

int frame = 0;
int width, height;
int oldX, oldY;

void cow_translate();
void cow_rotate();

// PA #2 의 수정된 부분 #1
// rotate, axis, delta 변수를 선언하였다.
// 만약 rotate 변수의 값이 0 이면 회전기능이 작동하지 않고,
//                         1 이면 회전기능이 작동한다.
// axis 변수는 값이 0이면 x 축, 1 이면 y 축, 2 이면 z 축이다.
// delta 변수는 평행 이동 거리를 가지고 있는 변수이다.
int rotate = 0;
int axis = 0;
double delta = 0;

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void drawAxis(float len){

		glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
		glBegin(GL_LINES);			// Start drawing lines.
			glColor3d(1, 0, 0);			// color of x-axis is red.
			glVertex3d(0, 0, 0);
			glVertex3d(len, 0, 0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
			glColor3d(0, 1, 0);			// color of y-axis is green.
			glVertex3d(0, 0, 0);
			glVertex3d(0, len, 0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
			glColor3d(0, 0, 1);			// color of z-axis is  blue.
			glVertex3d(0, 0, 0);
			glVertex3d(0, 0, len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
		glEnd();					// End drawing lines.
		glEnable(GL_LIGHTING);
}

void InitCamera(){

		// initialize camera frame transforms.
		for (int i = 0; i < cameraCount; i++){

			float * c = cameras[i];											// 'c' points the coordinate of i-th camera.
			wld2cam.push_back(Matrix());								// Insert {0} matrix to wld2cam vector.
			glPushMatrix();													// Push the current matrix of GL into stack.

			glLoadIdentity();												// Set the GL matrix Identity matrix.
			gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);		// Setting the coordinate of camera.
			glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam[i].matrix());			// Read the world-to-camera matrix computed by gluLookAt.
			cam2wld.push_back(wld2cam[i].inverse());						// Get the camera-to-world matrix.

			glPopMatrix();													// Transfer the matrix that was pushed the stack to GL.
		}

		cameraIndex = 0;
}

void drawCamera(){

	int i;
		// set viewing transformation.
		glLoadMatrixf(wld2cam[cameraIndex].matrix());

		// draw cameras.
		for (i = 0; i < (int)wld2cam.size(); i++){

			if (i != cameraIndex){

				glPushMatrix();													// Push the current matrix on GL to stack. The matrix is wld2cam[cameraIndex].matrix().
				glMultMatrixf(cam2wld[i].matrix());								// Multiply the matrix to draw i-th camera.

				glutSolidSphere(1.0, 10, 10);
				drawAxis(5);													// Draw x, y, and z axis.

				glPopMatrix();													// Call the matrix on stack. wld2cam[cameraIndex].matrix() in here.
			}
		}
}

/*********************************************************************************
* Draw 'cow' object.
**********************************************************************************/
void InitCow(){

	cow = new WaveFrontOBJ("cow.obj");

		// Make display list. After this, you can draw cow using 'cowID'.
		glPushMatrix();						// Push the current matrix of GL into stack.
		glLoadIdentity();					// Set the GL matrix Identity matrix.
		glTranslatef(0, -cow->bbmin.pos.y, -8);	// Set the location of cow.
		glRotatef(-90, 0, 1, 0);			// Set the direction of cow. These information are stored in the matrix of GL.
		glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());	// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
		glPopMatrix();						// Pop the matrix on stack to GL.
}

void drawCow(){

		glPushMatrix();		// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.

		// The information about location of cow to be drawn is stored in cow2wld matrix.
		// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
		glMultMatrixf(cow2wld.matrix());

		cow->Draw();						// Draw the cow on display list.
		drawAxis(5);						// Draw x, y, and z axis.

		glPopMatrix();			// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

/*********************************************************************************
* Draw floor on 3D plane.
**********************************************************************************/
void drawFloor(){

		if (frame == 0){
			// Initialization part.
			// After making checker-patterned texture, use this repetitively.

			// Insert color into checker[] according to checker pattern.
			const int size = 8;
			unsigned char checker[size*size * 3];

			for (int i = 0; i < size*size; i++){

				if (((i / size) ^ i) & 1){

					checker[3 * i + 0] = 100;
					checker[3 * i + 1] = 100;
					checker[3 * i + 2] = 100;
				}
				else{

					checker[3 * i + 0] = 200;
					checker[3 * i + 1] = 200;
					checker[3 * i + 2] = 200;
				}
			}

			// Make texture which is accessible through floorTexID. 
			glGenTextures(1, &floorTexID);
			glBindTexture(GL_TEXTURE_2D, floorTexID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
		}

		glDisable(GL_LIGHTING);

		// Set background color.

		glColor3d(0.35, 0.2, 0.1);

		// Draw background rectangle.
		glBegin(GL_POLYGON);
			glVertex3f(2000, -0.2, 2000);
			glVertex3f(2000, -0.2, -2000);
			glVertex3f(-2000, -0.2, -2000);
			glVertex3f(-2000, -0.2, 2000);
		glEnd();


		// Set color of the floor.
		// Assign checker-patterned texture.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, floorTexID);

		// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
		glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glVertex3d(-12, -0.1, -12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
			glTexCoord2d(1, 0);
			glVertex3d(12, -0.1, -12);		// Texture's (1,0) is bound to (12,-0.1,-12).
			glTexCoord2d(1, 1);
			glVertex3d(12, -0.1, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
			glTexCoord2d(0, 1);
			glVertex3d(-12, -0.1, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
		glEnd();

		glDisable(GL_TEXTURE_2D);

		drawAxis(5);				// Draw x, y, and z axis.
}

/*********************************************************************************
* Call this part whenever display events are needed.
* Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
* Or, user can occur the events by using glutPostRedisplay() function directly.
* this part is called in main() function by registering on glutDisplayFunc(display).
**********************************************************************************/
void display(){

		glClearColor(0, 0.6, 0.8, 1);									// Clear color setting

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear the screen

		drawCamera();													// and draw all of them.
		drawFloor();													// Draw floor.

		drawCow();														// Draw cow.

		glutSwapBuffers();

		frame++;
}

/*********************************************************************************
* Call this part whenever size of the window is changed.
* This part is called in main() function by registering on glutReshapeFunc(reshape).
**********************************************************************************/
void reshape(int w, int h){

		width = w;
		height = h;
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
		glLoadIdentity();                       // Reset The Projection Matrix
		float aspect = width / float(height);
		gluPerspective(45, aspect, 1, 1024);

		glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
		glLoadIdentity();                       // Reset The Projection Matrix
}

//------------------------------------------------------------------------------
void initialize(){

		// Set up OpenGL state
		glShadeModel(GL_FLAT);			 // Set Flat Shading
		glEnable(GL_DEPTH_TEST);         // Enables Depth Testing

		// Define lighting for the scene
		glEnable(GL_LIGHTING);
		float light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
		float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
		float light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glEnable(GL_LIGHT0);

		InitCamera();
		InitCow();
}

/*********************************************************************************
* Call this part whenever mouse button is clicked.
* This part is called in main() function by registering on glutMouseFunc(onMouseButton).
**********************************************************************************/
void onMouseButton(int button, int state, int x, int y){

		y = height - y - 1;

		if (button == GLUT_LEFT_BUTTON){

			if (state == GLUT_DOWN){

				printf("Left mouse click at (%d, %d)\n", x, y);

				// Save current clicked location of mouse here, and then use this on onMouseDrag function. 
				oldX = x;
				oldY = y;
			}
		}

		glutPostRedisplay();
}

/*********************************************************************************
* Call this part whenever user drags mouse.
* Input parameters x, y are coordinate of mouse on dragging.
* Value of global variables oldX, oldY is stored on onMouseButton,
* Then, those are used to verify value of x - oldX,  y - oldY to know its movement.
**********************************************************************************/
void onMouseDrag(int x, int y){

	// PA #2 의 수정된 부분 #2
	// 모든 카메라의 시점에서 소의 방향이 어떻던지 간에 잘 이동하게 만들기 위해 조금의 벡터 연산을 한다.
	// 마우스의 입력은 카메라가 보여주는 평면의 x, y 좌표로 표현된다. (마우스 드래그는 벡터로 표현)
	// 마우스 드래그의 2 차원 벡터를 실제 3 차원 좌표계 (체스판 좌표계) 에 대응하는 벡터로 변환하고,
	// 그 벡터를 다시 소의 좌표계에 내적하여, 실제 소가 움직이는 거리와 방향을 구하는 데 사용하였다.
	// 카메라의 위치 좌표를 E, 카메라가 바라보는 지점을 C
	// 카메라의 up vector 를 U, 마우스를 드래그한 상대적인 좌표 값을 deltaX, deltaY 라고 하자.
	// 우리는 화면 상에서 마우스를 드래그한 벡터 (deltaX, deltaY, 0) 를 실제 3 차원으로 변환한 mouseProj 벡터를 만들 것이다.
	// 먼저, 우리가 바라보는 화면의 x, y 축을 생각해보면
	// x 축은 벡터 EC 와 U 를 외적한 방향이고, y 축은 그냥 U 이다.
	// 따라서 마우스 드래그가 실제 3 차원으로 가기 위해선 deltaX * (EC cross U) + deltaY * U 를 계산해주면 된다.
	// 실제 코드에서는 mouseProj 과 cameraUp 벡터를 가지고, 위의 연산을 정규화 해주면서 진행하였다.
	// 이렇게 만들어진 최종 mouseProj 은 2 차원 화면 상의 드래그를 체스판 기준의 3 차원 벡터로 바꾸어 준 것이다.
	// 만약 소가 모든 축에 대해 움직이는 것이었다면, 바로 mouseProj 만큼 소를 이동하면 된다.
	// 그러나 우리는 한 가지 축에 대해서만 이동하기로 했기 때문에, 이 벡터를 소의 좌표축에 대해 내적해주고
	// 그 크기만큼 정해진 축으로 이동하면 된다.
	// 소의 좌표축을 cowVec 에 가져오고, 그 값과 mouseProj 을 내적한 값이 최종 이동 거리인 delta 가 된다.
	// 참고로 4번 카메라의 시점이 특수한 상황이라 소를 적절하게 회전하지 않은 상태에서는
	// x 축으로 소를 이동할 수 없다.
	// 마찬가지로 5번 카메라의 시점이 특수한 상황이라 소를 적절하게 회전하지 않은 상태에서는
	// y 축으로 소를 이동할 수 없다.
	// 이는 4, 5 번 카메라의 위치를 살짝 변경하면 그렇다는 것을 확인 할 수 있다.
	// 그리고 5번 카메라의 up vector 방향이 우리의 직관과 다르게 설정되어 있어서
	// x 축으로 이동할 때에는 z 축으로 드래그를 하고 (세로 드래그)
	// z 축으로 이동할 때에는 x 축으로 드래그를 해야 한다. (가로 드래그)

	float deltaX;
	float deltaY;
	Vector mouseProj(cameras[cameraIndex][3] - cameras[cameraIndex][0], cameras[cameraIndex][4] - cameras[cameraIndex][1], cameras[cameraIndex][5] - cameras[cameraIndex][2]);
	Vector cameraUp(cameras[cameraIndex][6], cameras[cameraIndex][7], cameras[cameraIndex][8]);

		y = height - y - 1;
		printf("in drag (%d, %d)\n", x - oldX, y - oldY);

		deltaX = x - oldX;
		deltaY = y - oldY;

		Vector deltaVec(deltaX, deltaY, 0);

		mouseProj = deltaX * mouseProj.CrossProduct(cameraUp);
		mouseProj.Normalize();
		cameraUp = deltaY * cameraUp;
		cameraUp.Normalize();
		mouseProj = mouseProj + cameraUp;
		mouseProj.Normalize();

		if(axis == 0){

			Vector cowVec(cow2wld.m[0][2], cow2wld.m[0][1], cow2wld.m[0][0]);

			delta = 0.1 * mouseProj.InnerProduct(cowVec);
		}
		else if (axis == 1){

			Vector cowVec(cow2wld.m[1][2], cow2wld.m[1][1], cow2wld.m[1][0]);

			delta = 0.1 * mouseProj.InnerProduct(cowVec);
		}
		else{

			Vector cowVec(cow2wld.m[2][2], cow2wld.m[2][1], cow2wld.m[2][0]);

			delta = 0.1 * mouseProj.InnerProduct(cowVec);
		}

		oldX = x;
		oldY = y;
		glutPostRedisplay();
}

/*********************************************************************************
* Call this part whenever user types keyboard.
* This part is called in main() function by registering on glutKeyboardFunc(onKeyPress).
**********************************************************************************/
void onKeyPress(unsigned char key, int x, int y){

		// PA #2 의 수정된 부분 #3
		// switch / case 문을 이용해서 각 키의 입력에 따라 이벤트를 처리한다.
		// 일단 기존의 카메라 시점 변경 기능도 여기로 옮겼다.
		// 'r' 또는 'R' 키를 누르면 rotate 변수의 값을 바꾸어, 회전 기능을 끄고 켠다.
		// 'x' 또는 'X' 키를 누르면 axis 변수의 값을 0 으로 바꾸어 x 축을 평행, 회전 이동을 하는 축으로 설정한다.
		// 'y' 또는 'Y' 키를 누르면 axis 변수의 값을 1 으로 바꾸어 y 축을 평행, 회전 이동을 하는 축으로 설정한다.
		// 'z' 또는 'Z' 키를 누르면 axis 변수의 값을 2 으로 바꾸어 z 축을 평행, 회전 이동을 하는 축으로 설정한다.
		switch(key){

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
				
				cameraIndex = key - '1';
				printf("Camera : %d\n", key - '1' + 1);

				break;
			
			case 'r':
			case 'R':

				rotate = (rotate + 1) % 2;
				
				if (rotate % 2){

					printf("Rotate is on\n");
				}
				else{

					printf("Rotate is off\n");
				}

				break;

			case 'x':
			case 'X':

				axis = 0;

				printf("x-axis is selected\n");

				break;

			case 'y':
			case 'Y':

				axis = 1;

				printf("y-axis is selected\n");

				break;

			case 'z':
			case 'Z':

				axis = 2;

				printf("z-axis is selected\n");

				break;

			default:

				break;
		}

		glutPostRedisplay();
}

void cow_rotate(){

		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(cow2wld.matrix()); //load current transformation matrix of object







		// PA #2 의 수정된 부분 #4
		// switch / case 문을 이용해서 현재 설정에 따라 소를 회전한다.
		// 먼저 회전 기능이 켜져 있는지 확인하고,
		// 회전 기능이 켜져있다면, 현재 설정된 축으로 회전을 한다.
		if (rotate){

			switch (axis){

				case 0: // x-axis
					
					glRotatef(1.0, 1.0, 0.0, 0.0);

					break;

				case 1: // y-axis

					glRotatef(1.0, 0.0, 1.0, 0.0);

					break;

				case 2: // z-axis

					glRotatef(1.0, 0.0, 0.0, 1.0);

					break;

				default:

					break;
			}
		}

		glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());
		glPopMatrix();
}

void cow_translate(){

		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(cow2wld.matrix()); //load current transformation matrix of object

		// PA #2 의 수정된 부분 #5
		// switch / case 문을 이용해서 소를 이동한다.
		// 먼저 회전 기능이 꺼져 있는지 확인하고,
		// 회전 기능이 꺼져있다면, 현재 설정된 축으로 이동한다.
		// 이동하는 거리는 delta 변수에 의해 결정된다.
		if (!rotate){

			switch (axis){

				case 0: // x-axis

					glTranslatef(delta, 0.0, 0.0);

					break;

				case 1: // y-axis

					glTranslatef(0.0, delta, 0.0);

					break;

				case 2: // z-axis

					glTranslatef(0.0, 0.0, delta);

					break;

				default:

					break;
			}
		}

		glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());
		glPopMatrix();
}

void idle() {

		// PA #2 의 수정된 부분 #6
		// cow_rotate, cow_translate 함수를 이용해 소의 움직임을 표현한다.
		// 두 함수에 의해, 소의 위치나 방향이 변화하고
		// drawCow 함수는 변한 상태를 가지고 다시 소를 그린다.
		// 그리고 translate 함수에서 사용한 delta 값을 초기화한다.
		// 그래야 마우스 입력이 끝나면 소도 같이 멈출 수 있다.
		cow_rotate();
		cow_translate();

		drawCow();
		delta = 0.0;

		glutPostRedisplay();
}

//------------------------------------------------------------------------------
void main(int argc, char* argv[]){

		width = 800;
		height = 600;
		frame = 0;
		glutInit(&argc, argv);							// Initialize openGL.
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	// Initialize display mode. This project will use float buffer and RGB color.
		glutInitWindowSize(width, height);				// Initialize window size.
		glutInitWindowPosition(100, 100);				// Initialize window coordinate.
		glutCreateWindow("PA2");

		glutDisplayFunc(display);						// Register display function to call that when drawing screen event is needed.
		glutReshapeFunc(reshape);						// Register reshape function to call that when size of the window is changed.
		glutKeyboardFunc(onKeyPress);					// Register onKeyPress function to call that when user presses the keyboard.
		glutMouseFunc(onMouseButton);					// Register onMouseButton function to call that when user moves mouse.
		glutMotionFunc(onMouseDrag);					// Register onMouseDrag function to call that when user drags mouse.

		glutIdleFunc(idle);

		initialize();									// Initialize the other thing.
		glutMainLoop();									// Execute the loop which handles events.
}