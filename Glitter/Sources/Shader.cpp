#include "Shader.hpp"

#include <iostream>

Shader::Shader(const char* filepath, GLenum shaderType)
    :
    m_shaderId(0),
    m_shaderType(shaderType),
    p_filepath(filepath)
{
    
}

Shader::~Shader()
{
    //
}

void Shader::init()
{
    m_shaderId = glCreateShader(m_shaderType);
    loadShaderFile(p_filepath);

    int success = 0;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &success);

    if (success = GL_FALSE)
    {
        throw std::runtime_error("Failed to compile shader!");
    }
}

void Shader::cleanup()
{
    glDeleteShader(m_shaderId);
}

GLuint Shader::getId()
{
    return m_shaderId;
}

void Shader::loadShaderFile(const char* filepath)
{
    FILE* pFile = fopen(filepath, "r");

    if (!pFile)
        throw std::runtime_error("Failed to open shader file!");

    fseek(pFile, 0, SEEK_END);
    size_t fileSize = ftell(pFile);
    rewind(pFile);

    char* pShaderCode = new char[fileSize]{};
    fread(pShaderCode, sizeof(char), fileSize, pFile);
    fclose(pFile);
    
    glShaderSource(m_shaderId, 1, (const char**)&pShaderCode, NULL);
    glCompileShader(m_shaderId);
    delete[] pShaderCode;
}

