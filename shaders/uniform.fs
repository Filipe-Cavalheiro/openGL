#version 430 core
layout (location = 0) flat in float color;

out vec4 FragColor;
void main(){
    FragColor = vec4(color, color, color, 1.0f);
}
