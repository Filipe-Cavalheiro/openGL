#version 430 core
layout (location = 0) in vec3 aPos;

layout (location = 0) flat out int State;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int state;

void main(){
    State = state; 
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
