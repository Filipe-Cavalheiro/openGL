#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h" // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_shader_content(const char* fileName);
void compile_shader(GLuint* shaderId, GLenum shaderType, const char* shaderFilePath);
void link_shader(GLuint vertexShaderID, GLuint fragmentShaderID, GLuint programID);

#endif
