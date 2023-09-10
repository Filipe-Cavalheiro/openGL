#version 430 core
layout (location = 0) in vec3 aPos;

layout (location = 0) flat out float Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float color;

void main(){
    Color = color; 
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
