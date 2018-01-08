#version 330 core

layout (location = 0) in vec3 aPos;
 
out vec3 Textcoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main () {

	Textcoords = aPos / 5.0;
	gl_Position = P * V * M * vec4(aPos, 1.0);
}