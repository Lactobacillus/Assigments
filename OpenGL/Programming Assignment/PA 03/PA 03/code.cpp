/*
*  Programming Assignment #3
*  Computer Graphics 2017
*  Author : Cho Youngwoo
*  2017.04.30
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

int cameraIndex, camID;
vector<Matrix> wld2cam, cam2wld;

// Variables for 'cow' object.
Matrix cow2wld;
WaveFrontOBJ* cow;
int cowID;

// Variables for 'bunny' object.
Matrix bunny2wld;
WaveFrontOBJ* bunny;
int bunnyID;

unsigned floorTexID;

int frame = 0;
int width, height;
int oldX, oldY;

void translate();
void rotate();

int selectMode;
bool picking = false;

// PA #3 �� ������ �κ� #1
// object, viewMode, transMode, rotateMode, delta ������ �����Ͽ���.
// object �� ������ ��ü�� �����Ѵ�.
// object == 0 �̸�, �Ҹ� ������ ���̰�,
// object == 1 �̸�, �䳢�� ������ ���̴�.
// viewMode �� modeling space �� ������ ������, viewing space �� ������ ���� �˷��ش�.
// viewMode == 0 �̸�, modeling space �̰�,
// viewMode == 1 �̸�, viewing space �̴�.
// transMode �� �����̵��� ���� ���� ���� ������.
// transMode == 0 �̸�, x ��ư�� ������ ���� �ൿ�� �Ѵ�.
// transMode == 1 �̸�, y ��ư�� ������ ���� �ൿ�� �Ѵ�.
// transMode == 2 �̸�, z ��ư�� ������ ���� �ൿ�� �Ѵ�.
// rotateMode �� ȸ�� ����� �����ϴ� ������.
// rotateMode == 0 �̸�, ȸ�� ����� ������.
// rotateMode == 1 �̸�, ȸ�� ����� ������.
// delta �� ���콺 �������� ũ�⸦ ��Ÿ���� ������.
// delta2 �� viewing space ���� xy ������� ������ �� �߰������� ����ϴ� ������.
int object = 0;
int viewMode = 0;
int transMode = 0;
int rotateMode = 0;
double delta = 0.0;
double delta2 = 0.0;

//------------------------------------------------------------------------------
void munge(int x, float& r, float& g, float& b){

		r = (x & 255) / float(255);
		g = ((x >> 8) & 255) / float(255);
		b = ((x >> 16) & 255) / float(255);
}

//------------------------------------------------------------------------------
int unmunge(float r, float g, float b){

	return (int(r) + (int(g) << 8) + (int(b) << 16));
}

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void drawAxis(float len) {

		glDisable(GL_LIGHTING);														// Lighting is not needed for drawing axis.
		glBegin(GL_LINES);															// Start drawing lines.
			glColor3d(1, 0, 0);														// color of x-axis is red.
			glVertex3d(0, 0, 0);
			glVertex3d(len, 0, 0);													// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
			glColor3d(0, 1, 0);														// color of y-axis is green.
			glVertex3d(0, 0, 0);
			glVertex3d(0, len, 0);													// Draw line(y-axis) from (0,0,0) to (0, len, 0).
			glColor3d(0, 0, 1);														// color of z-axis is  blue.
			glVertex3d(0, 0, 0);
			glVertex3d(0, 0, len);													// Draw line(z-axis) from (0,0,0) - (0, 0, len).
		glEnd();																	// End drawing lines.
		glEnable(GL_LIGHTING);
}

void InitCamera(){

		// initialize camera frame transforms.
		for (int i = 0; i < cameraCount; i++){

			float * c = cameras[i];													// 'c' points the coordinate of i-th camera.
		
			wld2cam.push_back(Matrix());											// Insert {0} matrix to wld2cam vector.
			glPushMatrix();															// Push the current matrix of GL into stack.

			glLoadIdentity();														// Set the GL matrix Identity matrix.
			gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);		// Setting the coordinate of camera.
			glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam[i].matrix());					// Read the world-to-camera matrix computed by gluLookAt.
			cam2wld.push_back(wld2cam[i].inverse());								// Get the camera-to-world matrix.

			glPopMatrix();															// Transfer the matrix that was pushed the stack to GL.
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
				
				glPushMatrix();														// Push the current matrix on GL to stack. The matrix is wld2cam[cameraIndex].matrix().
				glMultMatrixf(cam2wld[i].matrix());									// Multiply the matrix to draw i-th camera.
			
				if (selectMode == 0){												// selectMode == 1 means backbuffer mode.
			
					drawAxis(5);													// Draw x, y, and z axis.
				}
				else{

					float r, g, b;
				
					glDisable(GL_LIGHTING);											// Disable lighting in backbuffer mode.
					munge(i + 1, r, g, b);											// Match the corresponding (i+1)th color to r, g, b. You can change the color of camera on backbuffer.
					glColor3f(r, g, b);												// Set r, g, b the color of camera.
				}

				glutSolidSphere(1.0, 10, 10);

				glPopMatrix();														// Call the matrix on stack. wld2cam[cameraIndex].matrix() in here.
			}
		}
}

/*********************************************************************************
* Draw 'cow' object.
**********************************************************************************/
void InitCow(){

	cow = new WaveFrontOBJ("cow.obj");

		cowID = glGenLists(1);														// Create display lists
		glNewList(cowID, GL_COMPILE);												// Begin compiling the display list using cowID
		cow->Draw();																// Draw the cow on display list.
		glEndList();																// Terminate compiling the display list. Now, you can draw cow using 'cowID'.

		// Make display list. After this, you can draw cow using 'cowID'.
		glPushMatrix();																// Push the current matrix of GL into stack.
		glLoadIdentity();															// Set the GL matrix Identity matrix.
		glTranslatef(0, -cow->bbmin.pos.y, -8);										// Set the location of cow.
		glRotatef(-90, 0, 1, 0);													// Set the direction of cow. These information are stored in the matrix of GL.
		glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());							// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
		glPopMatrix();																// Pop the matrix on stack to GL.
}

void drawCow(){

		glPushMatrix();																// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.

		// The information about location of cow to be drawn is stored in cow2wld matrix.
		// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
		glMultMatrixf(cow2wld.matrix());

		if (selectMode == 0){														// selectMode == 1 means backbuffer mode.

			drawAxis(5);															// Draw x, y, and z axis.
		}
		else{

			float r, g, b;
		
			glDisable(GL_LIGHTING);													// Disable lighting in backbuffer mode.
			munge(32, r, g, b);														// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
			glColor3f(r, g, b);
		}
	
		glCallList(cowID);															// Draw cow. 

		glPopMatrix();																// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitBunny(){
	
	bunny = new WaveFrontOBJ("bunny.obj");

		bunnyID = glGenLists(1);													// Create display lists
		glNewList(bunnyID, GL_COMPILE);												// Begin compiling the display list using bunnyID
		bunny->Draw();																// Draw the bunny on display list.
		glEndList();																// Terminate compiling the display list. Now, you can draw beethovna using 'bunnyID'.

		// Make display list. After this, you can draw bunny using 'bunnyID'.
		glPushMatrix();																// Push the current matrix of GL into stack.
		glLoadIdentity();															// Set the GL matrix Identity matrix.
		glTranslatef(0, -bunny->bbmin.pos.y, 8);									// Set the location of bunny.
		glRotatef(180, 0, 1, 0);													// Set the direction of bunny. These information are stored in the matrix of GL.
		glGetFloatv(GL_MODELVIEW_MATRIX, bunny2wld.matrix());						// Read the modelview matrix about location and direction set above, and store it in bunny2wld matrix.
		glPopMatrix();																// Pop the matrix on stack to GL.
}

void drawBunny(){

		glPushMatrix();																// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing bunny.

		// The information about location of bunny to be drawn is stored in bunny2wld matrix.
		// If you change the value of the bunny2wld matrix or the current matrix, bunny would rotate or move.
		glMultMatrixf(bunny2wld.matrix());

		if (selectMode == 0){														// selectMode == 1 means backbuffer mode.

			drawAxis(8);															// Draw x, y, and z axis.
		}
		else{

			float r, g, b;
		
			glDisable(GL_LIGHTING);													// Disable lighting in backbuffer mode.
			munge(33, r, g, b);														// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
			glColor3f(r, g, b);
	}

		glCallList(bunnyID);														// Draw bunny. 

		glPopMatrix();																// Pop the matrix in stack to GL. Change it the matrix before drawing bunny.
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
		if (selectMode == 0){

			glColor3d(0.35, 0.2, 0.1);
		}
		else{

			// In backbuffer mode.
			float r, g, b;
			munge(34, r, g, b);
			glColor3f(r, g, b);
		}

		// Draw background rectangle.
		glBegin(GL_POLYGON);
			glVertex3f(-2000, -0.2, -2000);
			glVertex3f(-2000, -0.2, 2000);
			glVertex3f(2000, -0.2, 2000);
			glVertex3f(2000, -0.2, -2000);
		glEnd();


		// Set color of the floor.
		if (selectMode == 0){

			// Assign checker-patterned texture.
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, floorTexID);
		}
		else{
			// Assign color on backbuffer mode.
			float r, g, b;
		
			munge(35, r, g, b);
			glColor3f(r, g, b);
		}

		// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
		glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glVertex3d(-12, -0.1, -12);												// Texture's (0,0) is bound to (-12,-0.1,-12).
			glTexCoord2d(0, 1);
			glVertex3d(-12, -0.1, 12);												// Texture's (0,1) is bound to (-12,-0.1,12).
			glTexCoord2d(1, 1);
			glVertex3d(12, -0.1, 12);												// Texture's (1,1) is bound to (12,-0.1,12).
			glTexCoord2d(1, 0);
			glVertex3d(12, -0.1, -12);												// Texture's (1,0) is bound to (12,-0.1,-12).
		glEnd();

		if (selectMode == 0){

			glDisable(GL_TEXTURE_2D);
			drawAxis(5);															// Draw x, y, and z axis.
		}
}

/*********************************************************************************
* Call this part whenever display events are needed.
* Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
* Or, user can occur the events by using glutPostRedisplay() function directly.
* this part is called in main() function by registering on glutDisplayFunc(display).
**********************************************************************************/
void display(){

		if (selectMode == 0){

			glClearColor(0, 0.6, 0.8, 1);											// Clear color setting
		}
		else{

			glClearColor(0, 0, 0, 1);												// When the backbuffer mode, clear color is set to black
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);							// Clear the screen

		drawCamera();																// and draw all of them.
		drawFloor();																// Draw floor.

		drawCow();																	// Draw cow.
		drawBunny();

		glFlush();

		// If it is not backbuffer mode, swap the screen. In backbuffer mode, this is not necessary because it is not presented on screen.
		if (selectMode == 0){

			glutSwapBuffers();
		}

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

		glMatrixMode(GL_PROJECTION);												// Select The Projection Matrix
		glLoadIdentity();															// Reset The Projection Matrix
	
		float aspect = width / float(height);
		gluPerspective(45, aspect, 1, 1024);

		glMatrixMode(GL_MODELVIEW);													// Select The Modelview Matrix
		glLoadIdentity();															// Reset The Projection Matrix
}

//------------------------------------------------------------------------------
void initialize()
{
		// Set up OpenGL state
		glShadeModel(GL_SMOOTH);			 // Set Flat Shading
		glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
		// Initialize the matrix stacks
		reshape(width, height);
		// Define lighting for the scene
		float light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
		float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
		float light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glEnable(GL_LIGHT0);

		InitCamera();
		InitCow();
		InitBunny();
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

				// After drawing object on backbuffer, you can recognize which object is selected by reading pixel of (x, y).
				// Change the value of selectMode to 1, then draw the object on backbuffer when display() function is called. 
				selectMode = 1;
				display();
				glReadBuffer(GL_BACK);
				unsigned char pixel[3];
				glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
				int pixel_value = unmunge(pixel[0], pixel[1], pixel[2]);
				printf("pixel = %d\n", pixel_value);

				if(picking){

					// PA #3 �� ������ �κ� #2
					// ���� ��� �ȼ� ���� 32 �� ������,
					// �䳢�� ��� �ȼ� ���� 33 �� ���´�.
					// switch - case ���� �̿��� �ȼ� ������ �ҿ� �䳢�� �����Ѵ�.
					switch(pixel_value){

						case 32:

							object = 0;
							printf("pick cow\n");

							break;

						case 33:

							object = 1;
							printf("pick bunny\n");

							break;

						default:

							break;
					}

					picking = false;
				}

				selectMode = 0;
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

		// PA #3 �� ������ �κ� #3
		// �ϴ� ���� PA #2 ���� ��� ī�޶� ������ �����ϴ� �ڵ带 ���������Ƿ� ª�� �����Ѵ�.
		// �ڵ��� �ٽ��� �츮�� �ٶ󺸴� ȭ���� ���� 3D ���� ���� �� ����̰�, 
		// �츮�� �ٶ󺸴� ȭ���� X Y ���� ���� 3D ������ ���ͷ� ǥ���ϸ�,
		// 2������ ���콺 �������� 3D ������ ���ͷ� ��Ÿ�� �� �ִ�.
		// ����, Y �� ���ʹ� ī�޶��� �� �����̴�.
		// �׸��� X �� ���ʹ� ī�޶��� �� ���Ϳ� "ī�޶� ��ġ���� ī�޶� �ٶ󺸴� �������� ������ ����" �� �����̴�.
		// ������ ����ȭ�ϰ�, ũ�⸦ deltaX, deltaY ��ŭ���� �÷��ְ� ���ϸ� ���� ������ ���Ͱ� �ȴ�.
		// �� ���� ������ ���͸� ��ü�� ��ǥ��� �����ϸ�,
		// ��ü�� ���콺 �����ӿ� ���� ��� ������ �� ��ŭ �������� �ϴ� ��, ȸ���ϴ� ���� �� ����� �� �ִ�.
		float deltaX;
		float deltaY;
		Vector mouseProj(cameras[cameraIndex][3] - cameras[cameraIndex][0], cameras[cameraIndex][4] - cameras[cameraIndex][1], cameras[cameraIndex][5] - cameras[cameraIndex][2]);
		Vector cameraUp(cameras[cameraIndex][6], cameras[cameraIndex][7], cameras[cameraIndex][8]);

		y = height - y - 1;
		printf("in drag (%d, %d)\n", x - oldX, y - oldY);

		deltaX = x - oldX;
		deltaY = y - oldY;

		if(viewMode == 1 && rotateMode == 1){

			// viewing space ������ ȸ���� X ������ �ϱ� ������
			// �ܼ��� ���콺 �̵��� deltaY ������ ȸ�� ������ �����Ѵ�.
			if(deltaY > 0){

				delta = 1.0 * sqrt(deltaX * deltaX + deltaY * deltaY);
			}
			else{

				delta = -1.0 * sqrt(deltaX * deltaX + deltaY * deltaY);
			}
			
		}
		else if(viewMode == 0 && rotateMode == 0){

			// modeling space ������ �����̵�
			// PA #2 �� ���� �״�� ����Ѵ�.
			Vector deltaVec(deltaX, deltaY, 0);

			mouseProj = deltaX * mouseProj.CrossProduct(cameraUp);
			mouseProj.Normalize();
			cameraUp = deltaY * cameraUp;
			cameraUp.Normalize();
			mouseProj = mouseProj + cameraUp;
			mouseProj.Normalize();

			if(object == 0){							// cow

				if (transMode == 0){					// x-axis

					Vector cowVec(cow2wld.m[0][2], cow2wld.m[0][1], cow2wld.m[0][0]);

					delta = 0.2 * mouseProj.InnerProduct(cowVec);
				}
				else if (transMode == 1){				// y-axis

					Vector cowVec(cow2wld.m[1][2], cow2wld.m[1][1], cow2wld.m[1][0]);

					delta = 0.2 * mouseProj.InnerProduct(cowVec);
				}
				else{									// z-axis

					Vector cowVec(cow2wld.m[2][2], cow2wld.m[2][1], cow2wld.m[2][0]);

					delta = 0.2 * mouseProj.InnerProduct(cowVec);
				}
			}
			else{										// bunny

				if (transMode == 0){					// x-axis

					Vector bunnyVec(bunny2wld.m[0][2], bunny2wld.m[0][1], bunny2wld.m[0][0]);

					delta = 0.2 * mouseProj.InnerProduct(bunnyVec);
				}
				else if (transMode == 1){				// y-axis

					Vector bunnyVec(bunny2wld.m[1][2], bunny2wld.m[1][1], bunny2wld.m[1][0]);

					delta = 0.2 * mouseProj.InnerProduct(bunnyVec);
				}
				else{									// z-axis

					Vector bunnyVec(bunny2wld.m[2][2], bunny2wld.m[2][1], bunny2wld.m[2][0]);

					delta = 0.2 * mouseProj.InnerProduct(bunnyVec);
				}
			}

		}
		else if(viewMode == 1 && rotateMode == 0){

			if(transMode == 0 || transMode == 1){		// xy-plane

				// ȭ���� �߽ɿ����� �Ÿ��� delta ���� ��� ���谡 �ְ�����,
				// �����ؼ� �׳� deltaX �� deltaY ���� �״�� ����Ѵ�.
				delta = deltaX;
				delta2 = deltaY;
			}
			else{										// z-axis

				// viewing space ������ Z �� �̵��� ���������� �ʱ� ������
				// �ܼ��� ���콺 �̵��� deltaY ������ �̵� ������ �����Ѵ�.
				// �̶�, �̵� ������ ũ��� deltaX, deltaY ��� ����Ѵ�.
				if(deltaY > 0){

					delta = 0.003 * sqrt(deltaX * deltaX + deltaY * deltaY);
				}
				else{

					delta = -0.003 * sqrt(deltaX * deltaX + deltaY * deltaY);
				}
			}
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

		// PA #3 �� ������ �κ� #4
		// switch - case ���� �̿��� Ű �Է¿� ���� �ൿ�� �Ѵ�.
		// ����, ī�޶� ���� ���浵 ����� �Ű��.
		// M, m Ű�� ������ modeling space ��带 �����Ѵ�. (viewMode = 0)
		// V, v Ű�� ������ viewing space ��带 �����Ѵ�.  (viewMode = 1)
		// X, x Ű�� ������ x �� ��带 �����Ѵ�.           (transMode = 0)
		// Y, y Ű�� ������ y �� ��带 �����Ѵ�.           (transMode = 1)
		// Z, z Ű�� ������ z �� ��带 �����Ѵ�.           (transMode = 2)
		// R, r Ű�� ������ ȸ�� ��带 �����Ѵ�.           (rorateMode = 0 / 1)
		// P, p Ű�� ������ ������Ʈ ������ �Ѵ�.			(picking = true)
		switch(key){

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':

				cameraIndex = key - '1';

				break;

			case 'M':
			case 'm':

				viewMode = 0;
				printf("modeling space\n");

				break;

			case 'V':
			case 'v':

				viewMode = 1;
				printf("viewing space\n");

				break;

			case 'X':
			case 'x':

				transMode = 0;
				printf("button : x\n");

				break;

			case 'Y':
			case 'y':

				transMode = 1;
				printf("button : y\n");

				break;

			case 'Z':
			case 'z':

				transMode = 2;
				printf("button : z\n");

				break;

			case 'R':
			case 'r':

				rotateMode = (rotateMode + 1) % 2;
				printf("button : r\n");

				break;

			case 'P':
			case 'p':

				picking = true;

				break;

			default:

				break;
		}

		glutPostRedisplay();
}

void rotate(){

		glPushMatrix();
		glLoadIdentity();

		// PA #3 �� ������ �κ� #5
		// switch - case ���� �̿��� �ҿ� �䳢�� �����Ѵ�.
		// ����, modeling space �� ��쿡 �׳� PA2 ó�� �����Ͽ���.
		// �� ���� viewing space �� ȸ���� �����ؾ� �Ѵ�.
		// ���� ȭ�� ���� X ���� ��Ÿ���� ���͸� ���ϱ� ���ؼ�
		// ī�޶��� �����Ϳ� ī�޶��� ��ġ���� �ٶ󺸴� ������ ������ ���͸� �����´�.
		// �� ������ ������ �ٷ� ȭ�� ���� X �� ���Ͱ� �ȴ�.
		// �� ���͸� ������ �ϴ� ȸ���� �ϸ� �츮�� ���ϴ� ȸ���� �ȴ�.
		// �׷��� ���ؼ� ȸ�� ���� ��ü�� ��ǥ�迡 ���� ������ ��Ÿ���� �Ѵ�.
		// �𵨺� ��Ʈ������ �����ͼ� ��ü ������ X, Y, Z �� ���͸� �˾Ƴ���,
		// ������ �� ���Ϳ� ȸ�� �� ���͸� �������ָ�,
		// �츮�� �ٶ󺸴� X �������� ȸ���� ���� ȸ�� ����
		// ��ü�� ��ǥ�迡�� ��Ÿ������.
		// �̷��� ���� ȸ�� ������ ���콺 �̵� �Ÿ��� ���� delta ���� ����ϰ� ȸ������.
		// �׸��� �̷��� ȸ���� ��Ʈ������ ��ü�� �������ָ� �ȴ�.
		switch (object){

			case 0:		// cow

				glMultMatrixf(cow2wld.matrix());

				break;

			case 1:		// bunny

				glMultMatrixf(bunny2wld.matrix());

				break;

			default:

				break;
		}

		if (rotateMode == 1){

			if (viewMode == 0){		// modeling space

				// PA #2 �� ���� �״�� ����Ѵ�.
				switch (transMode){

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
			else{					// viewing space

				// ȭ���� X ���� ����ϱ� ���� ī�޶��� �� ���Ϳ� ī�޶��� ��ġ���� �ٶ󺸴� ������ ������ ���͸� �����Ѵ�.
				// ������ ���� (ȭ�� X ��) �� ��ü�� ��ǥ�迡 �����ְ� ȸ���� �ؾ� �Ѵ�.
				// ��ü�� X, Y, Z �� ���͸� ��������, ������ ��ü �� ���͸� ȭ�� X �� ���Ϳ� �������ش�.
				// ������ ���� ���� ��ü�� ȭ�� �� X ������ ȸ���ϱ� ���� ȸ�� ���� ������ �ȴ�.
				Matrix rotation;

				glGetFloatv(GL_MODELVIEW_MATRIX, rotation.matrix());

				Vector cameraToView(cameras[cameraIndex][3] - cameras[cameraIndex][0], cameras[cameraIndex][4] - cameras[cameraIndex][1], cameras[cameraIndex][5] - cameras[cameraIndex][2]);
				Vector cameraUp(cameras[cameraIndex][6], cameras[cameraIndex][7], cameras[cameraIndex][8]);
				Vector x(rotation.m[0][0], rotation.m[0][1], rotation.m[0][2]);
				Vector y(rotation.m[1][0], rotation.m[1][1], rotation.m[1][2]);
				Vector z(rotation.m[2][0], rotation.m[2][1], rotation.m[2][2]);

				x.Normalize();
				y.Normalize();
				z.Normalize();

				cameraToView = cameraToView.CrossProduct(cameraUp);
				cameraToView.Normalize();

				printf("%lf\n", delta);

				glRotatef(0.5 * delta, cameraToView.InnerProduct(x), cameraToView.InnerProduct(y), cameraToView.InnerProduct(z));
			}
		}

		switch (object){

			case 0:

				glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());

				break;

			case 1:

				glGetFloatv(GL_MODELVIEW_MATRIX, bunny2wld.matrix());

				break;

			default:

				break;
		}

		glPopMatrix();
}

void translate(){
		
		glPushMatrix();
		glLoadIdentity();

		// PA #3 �� ������ �κ� #6
		// switch - case ���� �̿��� �ҿ� �䳢�� �����Ѵ�.
		// ����, modeling space �� ��쿡 �׳� PA2 ó�� �����Ͽ���.
		// �� ���� viewing space �� ���� �̵��̴�.
		// XY ��鿡 ���� ���� �̵��� ȭ���� X, Y ���� ����ϰ�,
		// �� ������ ���콺�� �̵��� ũ�⸦ �����ְ� �����ش�.
		// �׷��� ����� ���͸� ��ü�� ��ǥ��� ������ ��ŭ �̵��ϸ� �ȴ�.
		// Z �࿡ ���� ���� �̵��� "ī�޶��� ��ġ���� ī�޶� �ٶ󺸴� �������� ������ ����" �� ����Ѵ�.
		// �� ���͸� ��ü�� ��ǥ��� ������ ��ŭ �̵��ϸ� �ȴ�.
		switch (object){

			case 0:		// cow

				glMultMatrixf(cow2wld.matrix());

				break;

			case 1:		// bunny

				glMultMatrixf(bunny2wld.matrix());

				break;

			default:

				break;
		}

		if (rotateMode == 0){

			if (viewMode == 0){		// modeling space

				switch (transMode){

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
			else{					// viewing space

				if (transMode == 0 || transMode == 1){	// xy-plane


					// ȭ���� X, Y ���� �� ���Ϳ� "ī�޶��� ��ġ���� ī�޶� �ٶ󺸴� �������� ������ ����" �� ������ �����.
					// X ���� delta ��ŭ �̵��ϰ�, Y ���� delta2 ��ŭ �̵��Ѵٰ� �ϰ�, ���� ������.
					// �ƹ�ư �̵��ϴ� ���͸� ���ϸ�, �� ���͸� ��ü�� ��ǥ�迡 ������� �Ѵ�.
					// �翬�� ������ ���ָ� �ȴ�.
					Matrix transformation;

					glGetFloatv(GL_MODELVIEW_MATRIX, transformation.matrix());

					Vector cameraToView(cameras[cameraIndex][3] - cameras[cameraIndex][0], cameras[cameraIndex][4] - cameras[cameraIndex][1], cameras[cameraIndex][5] - cameras[cameraIndex][2]);
					Vector cameraUp(cameras[cameraIndex][6], cameras[cameraIndex][7], cameras[cameraIndex][8]);
					Vector x(transformation.m[0][0], transformation.m[0][1], transformation.m[0][2]);
					Vector y(transformation.m[1][0], transformation.m[1][1], transformation.m[1][2]);
					Vector z(transformation.m[2][0], transformation.m[2][1], transformation.m[2][2]);

					cameraToView = delta * cameraToView.CrossProduct(cameraUp);
					cameraToView.Normalize();
					cameraUp = delta2 * cameraUp;
					cameraUp.Normalize();
					cameraToView = cameraToView + cameraUp;
					cameraToView.Normalize();

					x.Normalize();
					y.Normalize();
					z.Normalize();

					glTranslatef(0.18 * cameraToView.InnerProduct(x), 0.18 * cameraToView.InnerProduct(y), 0.18 * cameraToView.InnerProduct(z));
				}
				else{									// z-axis

					// ȭ�鿡�� Ƣ����� ������ Z ���� "ī�޶��� ��ġ���� ī�޶� �ٶ󺸴� �������� ������ ����" �� ����.
					// �� ������ delta ��ŭ �����δٰ� �ϰ�, ��ü�� ��ǥ�迡 ������ �ؼ� �̵��ϸ� �ȴ�.
					Matrix transformation;

					glGetFloatv(GL_MODELVIEW_MATRIX, transformation.matrix());

					Vector cameraToView(cameras[cameraIndex][3] - cameras[cameraIndex][0], cameras[cameraIndex][4] - cameras[cameraIndex][1], cameras[cameraIndex][5] - cameras[cameraIndex][2]);
					Vector x(transformation.m[0][0], transformation.m[0][1], transformation.m[0][2]);
					Vector y(transformation.m[1][0], transformation.m[1][1], transformation.m[1][2]);
					Vector z(transformation.m[2][0], transformation.m[2][1], transformation.m[2][2]);

					x.Normalize();
					y.Normalize();
					z.Normalize();

					glTranslatef(delta * cameraToView.InnerProduct(x), delta * cameraToView.InnerProduct(y), delta * cameraToView.InnerProduct(z));
				}
			}
		}

		switch (object){

			case 0:

				glGetFloatv(GL_MODELVIEW_MATRIX, cow2wld.matrix());

				break;

			case 1:

				glGetFloatv(GL_MODELVIEW_MATRIX, bunny2wld.matrix());

				break;

			default:

				break;
		}

		glPopMatrix();
}

void idle(){

		// PA #3 �� ������ �κ� #7
		// translate �� rotate �Լ��� �θ���, ��ü�� �ٽ� �׷�����.
		// �� ���� ������ �ʱ�ȭ ���ָ� �ȴ�.
		translate();
		rotate();

		drawCow();
		drawBunny();

		delta = 0.0;
		delta2 = 0.0;

		glutPostRedisplay();
}

//------------------------------------------------------------------------------
void main(int argc, char* argv[]){

		width = 800;
		height = 600;
		frame = 0;
	
		glutInit(&argc, argv);														// Initialize openGL.
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);								// Initialize display mode. This project will use float buffer and RGB color.
		glutInitWindowSize(width, height);											// Initialize window size.
		glutInitWindowPosition(100, 100);											// Initialize window coordinate.
		glutCreateWindow("PA3 - 2014210085 ������");

		glutDisplayFunc(display);													// Register display function to call that when drawing screen event is needed.
		glutReshapeFunc(reshape);													// Register reshape function to call that when size of the window is changed.
		glutKeyboardFunc(onKeyPress);												// Register onKeyPress function to call that when user presses the keyboard.
		glutMouseFunc(onMouseButton);												// Register onMouseButton function to call that when user moves mouse.
		glutMotionFunc(onMouseDrag);												// Register onMouseDrag function to call that when user drags mouse.

		glutIdleFunc(idle);

		initialize();																// Initialize the other thing.
		glutMainLoop();																// Execute the loop which handles events.
}