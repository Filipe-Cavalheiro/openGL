#version 430 core
layout (location = 0) flat in int state;

out vec4 FragColor;
void main(){
    if(state == 0)  //unchecked
        FragColor = vec4(0.075f, 0.561f, 0.294, 1.0f);
    else if(state == 1) //walls
        FragColor = vec4(0.455f, 0.478f, 0.467f, 1.0f);
    else if(state == 2) //start
        FragColor = vec4(1.0f, 0.604f, 0.2f, 1.0f);
    else if(state == 3)  // finish
        FragColor = vec4(0.573f, 0.2f, 1.0f, 1.0f);
    else if(state == 4)  // checked
        FragColor = vec4(0.2f, 0.886f, 1.0f, 1.0f);
    else if(state == 5)  //closest path
        FragColor = vec4(0.98f, 0.322f, 0.106f, 1.0f);
}
