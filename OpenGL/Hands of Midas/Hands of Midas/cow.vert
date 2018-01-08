#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexnormal;
layout(location = 2) in int renderbuffer1;
layout(location = 3) in int renderbuffer2;		// state before 1 turn

out vec3 L;
out vec3 VV;
out vec3 N;
out vec3 verpos;
flat out int render1;
flat out int render2;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 LightPosW;



void main(){


   gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
   verpos = (M*vec4(vertexPosition_modelspace,1)).xyz;
   vec3 vertexposition = (V*M*vec4(vertexPosition_modelspace,1)).xyz;
   VV = vec3(0,0,0) - vertexposition;

   vec3 lightposition = (V * vec4(LightPosW,1)).xyz;
   L = VV+lightposition;

   N = (V * M * vec4(vertexnormal, 0)).xyz;

   render1 = renderbuffer1;
   render2 = renderbuffer2;
}
