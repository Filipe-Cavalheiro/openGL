#version 430 core
out vec4 FragColor;
  
layout(location = 0) in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main(){
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2);
}
