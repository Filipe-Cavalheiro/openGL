#version 430 core
layout (location = 0) flat in int state;

out vec4 FragColor;
void main(){
        FragColor = vec4(state, state, state, 1.0f);
}
