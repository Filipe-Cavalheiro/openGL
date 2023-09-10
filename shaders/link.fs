#version 430 core
layout (location = 0) flat in int state;

out vec4 FragColor;
void main(){
    float color = state/255;
    FragColor = vec4(color, color, color, 1.0f);
}

