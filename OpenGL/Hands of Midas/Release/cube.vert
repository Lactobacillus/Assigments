/* cube.vert
 * Computer Graphics 2017
 * Author : Lee Sora
 * Author : Cho Youngwoo
 * Author : Choi Moonyoung
 * 2017.06.06
*/

#version 330 core

layout (location = 0) in vec3 aPos;
 
out vec3 Textcoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main () {

	Textcoords = aPos;
	gl_Position = P * V * M * vec4(aPos, 1.0);
}