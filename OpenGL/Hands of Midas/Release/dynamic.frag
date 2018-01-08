#version 330 core

in vec3 Textcoords;

out vec4 color;

uniform samplerCube text;

void main () {

	color = texture(text, normalize(Textcoords));
}