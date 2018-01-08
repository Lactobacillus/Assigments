/* bunny.vert
 * Computer Graphics 2017
 * Author : Lee Sora
 * Author : Cho Youngwoo
 * Author : Choi Moonyoung
 * 2017.06.12
*/

#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexnormal;

out vec3 VV;
out vec3 N;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main () {

	gl_Position =  P * V * M * vec4(vertexPosition_modelspace, 1);
	vec3 vertexposition = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
	VV = vec3(0.0, 0.0, 0.0) - vertexposition;

	N = (V * M * vec4(vertexnormal, 0)).xyz;
}