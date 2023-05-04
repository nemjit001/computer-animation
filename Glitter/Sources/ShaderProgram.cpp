#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram()
    :
    m_shaderProgram(0)
{
    
}

void ShaderProgram::init()
{
    m_shaderProgram = glCreateProgram();
}

void ShaderProgram::cleanup()
{
    glDeleteProgram(m_shaderProgram);
}

void ShaderProgram::registerShader(Shader& shader)
{
    glAttachShader(m_shaderProgram, shader.getId());
}

void ShaderProgram::use()
{
    glLinkProgram(m_shaderProgram);
    glUseProgram(m_shaderProgram);
}

