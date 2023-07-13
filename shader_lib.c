/* Loads the content of a GLSL Shader file into a char* variable */
#include "shader_lib.h"

char* get_shader_content(const char* fileName){
    FILE *fp;
    uint32_t size = 0;
    char* shaderContent;

    /* Read File to get size */
    fp = fopen(fileName, "rb");
     if (fp == NULL) {
        printf("Failed to open shader file: %s\n", fileName);
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(fileName, "r");
    shaderContent = memset(malloc(size), '\0', size);
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

void compile_shader(GLuint* shaderId, GLenum shaderType, const char* shaderFilePath){
    GLint isCompiled = 0;
    /* Calls the Function that loads the Shader source code from a file */
    const char* shaderSource = get_shader_content(shaderFilePath); 

    *shaderId = glCreateShader(shaderType);
    if(*shaderId == 0) {
        printf("COULD NOT LOAD SHADER: %s!\n", shaderFilePath);
    }

    glShaderSource(*shaderId, 1, (const char**)&shaderSource, NULL);
    glCompileShader(*shaderId);
    glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &isCompiled);

    if(isCompiled == GL_FALSE) {
        GLint maxLength = 255;
        char errorLog[maxLength];
        glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        glGetShaderInfoLog(*shaderId, maxLength, &maxLength, errorLog);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        printf("Shader Compiler Error: %s\n", shaderFilePath);
        printf("Error: %s\n", errorLog);
        glDeleteShader(*shaderId); // Don't leak the shader.
        return;    
    }
}

void link_shader(GLuint vertexShaderID, GLuint fragmentShaderID, GLuint programID){
    GLint isLinked = 0;

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE) {
        GLint maxLength = 0;
        char* infoLog = malloc(1024);
        printf("Shader Program Linker Error\n");

        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
        glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

        printf("%s\n", infoLog);

        glDeleteProgram(programID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        free(infoLog);

        return;
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}
