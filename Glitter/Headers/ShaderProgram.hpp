#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM
#pragma once

#include "Shader.hpp"

class ShaderProgram
{
public:
    ShaderProgram();
    void init();
    void cleanup();
    void registerShader(Shader& shader);
    void use();

private:
    GLuint m_shaderProgram;
};

#endif // SHADER_PROGRAM
