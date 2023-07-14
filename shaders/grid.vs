#version 430 core
layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out mat4 FragView;
layout(location = 6) out mat4 FragProjection;

uniform mat4 view;
uniform mat4 projection;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection){
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return (unprojectedPoint.xyz / unprojectedPoint.w);
}

void main(){
    nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    FragView = view;
    FragProjection = projection;
    gl_Position = vec4(aPos, 1.0);
}
