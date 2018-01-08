/* bunny.frag
 * Computer Graphics 2017
 * Author : Lee Sora
 * Author : Cho Youngwoo
 * Author : Choi Moonyoung
 * 2017.06.12
*/

#version 330 core

in vec3 VV;
in vec3 N;

out vec4 color;

uniform samplerCube text;

void main () {

	vec3 rmv = reflect(normalize(-VV), N);
	vec4 reflectenv = texture(text, rmv);

	color = reflectenv;
}