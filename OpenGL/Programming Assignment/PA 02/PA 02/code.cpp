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

// PA #2 �� ������ �κ� #1
// rotate, axis, delta ������ �����Ͽ���.
// ���� rotate ������ ���� 0 �̸� ȸ������� �۵����� �ʰ�,
//                         1 �̸� ȸ������� �۵��Ѵ�.
// axis ������ ���� 0�̸� x ��, 1 �̸� y ��, 2 �̸� z ���̴�.
// delta ������ ���� �̵� �Ÿ��� ������ �ִ� �����̴�.
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

	// PA #2 �� ������ �κ� #2
	// ��� ī�޶��� �������� ���� ������ ����� ���� �� �̵��ϰ� ����� ���� ������ ���� ������ �Ѵ�.
	// ���콺�� �Է��� ī�޶� �����ִ� ����� x, y ��ǥ�� ǥ���ȴ�. (���콺 �巡�״� ���ͷ� ǥ��)
	// ���콺 �巡���� 2 ���� ���͸� ���� 3 ���� ��ǥ�� (ü���� ��ǥ��) �� �����ϴ� ���ͷ� ��ȯ�ϰ�,
	// �� ���͸� �ٽ� ���� ��ǥ�迡 �����Ͽ�, ���� �Ұ� �����̴� �Ÿ��� ������ ���ϴ� �� ����Ͽ���.
	// ī�޶��� ��ġ ��ǥ�� E, ī�޶� �ٶ󺸴� ������ C
	// ī�޶��� up vector �� U, ���콺�� �巡���� ������� ��ǥ ���� deltaX, deltaY ��� ����.
	// �츮�� ȭ�� �󿡼� ���콺�� �巡���� ���� (deltaX, deltaY, 0) �� ���� 3 �������� ��ȯ�� mouseProj ���͸� ���� ���̴�.
	// ����, �츮�� �ٶ󺸴� ȭ���� x, y ���� �����غ���
	// x ���� ���� EC �� U �� ������ �����̰�, y ���� �׳� U �̴�.
	// ���� ���콺 �巡�װ� ���� 3 �������� ���� ���ؼ� deltaX * (EC cross U) + deltaY * U �� ������ָ� �ȴ�.
	// ���� �ڵ忡���� mouseProj �� cameraUp ���͸� ������, ���� ������ ����ȭ ���ָ鼭 �����Ͽ���.
	// �̷��� ������� ���� mouseProj �� 2 ���� ȭ�� ���� �巡�׸� ü���� ������ 3 ���� ���ͷ� �ٲپ� �� ���̴�.
	// ���� �Ұ� ��� �࿡ ���� �����̴� ���̾��ٸ�, �ٷ� mouseProj ��ŭ �Ҹ� �̵��ϸ� �ȴ�.
	// �׷��� �츮�� �� ���� �࿡ ���ؼ��� �̵��ϱ�� �߱� ������, �� ���͸� ���� ��ǥ�࿡ ���� �������ְ�
	// �� ũ�⸸ŭ ������ ������ �̵��ϸ� �ȴ�.
	// ���� ��ǥ���� cowVec �� ��������, �� ���� mouseProj �� ������ ���� ���� �̵� �Ÿ��� delta �� �ȴ�.
	// ����� 4�� ī�޶��� ������ Ư���� ��Ȳ�̶� �Ҹ� �����ϰ� ȸ������ ���� ���¿�����
	// x ������ �Ҹ� �̵��� �� ����.
	// ���������� 5�� ī�޶��� ������ Ư���� ��Ȳ�̶� �Ҹ� �����ϰ� ȸ������ ���� ���¿�����
	// y ������ �Ҹ� �̵��� �� ����.
	// �̴� 4, 5 �� ī�޶��� ��ġ�� ��¦ �����ϸ� �׷��ٴ� ���� Ȯ�� �� �� �ִ�.
	// �׸��� 5�� ī�޶��� up vector ������ �츮�� ������ �ٸ��� �����Ǿ� �־
	// x ������ �̵��� ������ z ������ �巡�׸� �ϰ� (���� �巡��)
	// z ������ �̵��� ������ x ������ �巡�׸� �ؾ� �Ѵ�. (���� �巡��)

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

		// PA #2 �� ������ �κ� #3
		// switch / case ���� �̿��ؼ� �� Ű�� �Է¿� ���� �̺�Ʈ�� ó���Ѵ�.
		// �ϴ� ������ ī�޶� ���� ���� ��ɵ� ����� �Ű��.
		// 'r' �Ǵ� 'R' Ű�� ������ rotate ������ ���� �ٲپ�, ȸ�� ����� ���� �Ҵ�.
		// 'x' �Ǵ� 'X' Ű�� ������ axis ������ ���� 0 ���� �ٲپ� x ���� ����, ȸ�� �̵��� �ϴ� ������ �����Ѵ�.
		// 'y' �Ǵ� 'Y' Ű�� ������ axis ������ ���� 1 ���� �ٲپ� y ���� ����, ȸ�� �̵��� �ϴ� ������ �����Ѵ�.
		// 'z' �Ǵ� 'Z' Ű�� ������ axis ������ ���� 2 ���� �ٲپ� z ���� ����, ȸ�� �̵��� �ϴ� ������ �����Ѵ�.
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







		// PA #2 �� ������ �κ� #4
		// switch / case ���� �̿��ؼ� ���� ������ ���� �Ҹ� ȸ���Ѵ�.
		// ���� ȸ�� ����� ���� �ִ��� Ȯ���ϰ�,
		// ȸ�� ����� �����ִٸ�, ���� ������ ������ ȸ���� �Ѵ�.
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

		// PA #2 �� ������ �κ� #5
		// switch / case ���� �̿��ؼ� �Ҹ� �̵��Ѵ�.
		// ���� ȸ�� ����� ���� �ִ��� Ȯ���ϰ�,
		// ȸ�� ����� �����ִٸ�, ���� ������ ������ �̵��Ѵ�.
		// �̵��ϴ� �Ÿ��� delta ������ ���� �����ȴ�.
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

		// PA #2 �� ������ �κ� #6
		// cow_rotate, cow_translate �Լ��� �̿��� ���� �������� ǥ���Ѵ�.
		// �� �Լ��� ����, ���� ��ġ�� ������ ��ȭ�ϰ�
		// drawCow �Լ��� ���� ���¸� ������ �ٽ� �Ҹ� �׸���.
		// �׸��� translate �Լ����� ����� delta ���� �ʱ�ȭ�Ѵ�.
		// �׷��� ���콺 �Է��� ������ �ҵ� ���� ���� �� �ִ�.
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