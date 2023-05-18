#include "Shader.hpp"

#include <iostream>

Shader::Shader()
    :
    m_programId(0)
{
    //
}

Shader::~Shader()
{
    //
}

void Shader::init()
{
    m_programId = glCreateProgram();
}

void Shader::cleanup()
{
    glDeleteProgram(m_programId);
}

unsigned int Shader::getUniformLocation(std::string uniformName) {
    return glGetUniformLocation(m_programId, uniformName.c_str());
}

Shader& Shader::registerShader(const char* filepath, GLenum shaderType)
{
    GLuint shaderId = glCreateShader(shaderType);
    FILE* pFile = fopen(filepath, "r");

    if (!pFile)
        throw std::runtime_error("Failed to open shader file!");

    fseek(pFile, 0, SEEK_END);
    size_t fileSize = ftell(pFile);
    rewind(pFile);

    char* pShaderCode = new char[fileSize + 1] {};
    fread(pShaderCode, sizeof(char), fileSize, pFile);
    fclose(pFile);

    glShaderSource(shaderId, 1, (const char**)&pShaderCode, NULL);
    glCompileShader(shaderId);
    delete[] pShaderCode;

    int success = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "Error in compilation of shader. Info log:\n" << infoLog << std::endl;
        throw std::runtime_error("Failed to compile shader!");
    }

    glAttachShader(m_programId, shaderId);
    glDeleteShader(shaderId);

    return *this;
}

Shader& Shader::link()
{
    int success = 0;

    glLinkProgram(m_programId);
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        throw std::runtime_error("Failed to link shader program!");
    }

    return *this;
}

Shader& Shader::use()
{
    glUseProgram(m_programId);

    return *this;
}
