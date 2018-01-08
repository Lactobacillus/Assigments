/* cube.frag
 * Computer Graphics 2017
 * Author : Lee Sora
 * Author : Cho Youngwoo
 * Author : Choi Moonyoung
 * 2017.06.06
*/

#version 330 core

in vec3 Textcoords;

out vec4 color;

uniform samplerCube text;

void main () {

	color = texture(text, Textcoords);
}