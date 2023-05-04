#ifndef SHADER
#define SHADER
#pragma once

#include <glad/glad.h>

class Shader
{
public:
    Shader(const char* filepath, GLenum shaderType);
    ~Shader();

    void init();
    void cleanup();
    GLuint getId();
private:
    void loadShaderFile(const char* filepath);

private:
    GLuint m_shaderId;
    GLenum m_shaderType;
    const char* p_filepath;
};

#endif // SHADER
