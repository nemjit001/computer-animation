#include "Shader.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

Shader& Shader::registerShader(const char* filepath, GLenum shaderType)
{
    GLuint shaderId = glCreateShader(shaderType);
    FILE* pFile = fopen(filepath, "r");

    if (!pFile)
        throw std::runtime_error("Failed to open shader file!");

    fseek(pFile, 0, SEEK_END);
    size_t fileSize = ftell(pFile);
    rewind(pFile);

    char* pShaderCode = new char[fileSize + 1]{};
    fread(pShaderCode, sizeof(char), fileSize, pFile);
    fclose(pFile);

    glShaderSource(shaderId, 1, (const char**)&pShaderCode, NULL);
    glCompileShader(shaderId);
    delete[] pShaderCode;

    int success = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
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

GLuint Shader::getShaderID()
{
    return m_programId;
}

void Shader::setInt(const std::string& name, int value) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniform1i(uniform_location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniform1f(uniform_location, value);
}

void Shader::setMat4(const std::string& name, const glm::mat4 mat) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, const glm::vec3 vec) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniform3fv(uniform_location, 1, glm::value_ptr(vec));
}

void Shader::setMat4Vector(const std::string& name, const std::vector<glm::mat4> mat_vec) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniformMatrix4fv(uniform_location, (GLsizei)mat_vec.size(), GL_FALSE, glm::value_ptr(mat_vec[0]));
}

void Shader::setMat4x2Vector(const std::string& name, const std::vector<glm::mat4x2> mat_vec) const
{
    int uniform_location = glGetUniformLocation(m_programId, name.c_str());

    if (uniform_location == -1)
    {
        std::cout << "ERROR:SHADER::PROGRAM::UNIFORM:: Location with Name " << name << " Not Found or is Not in Use!" << std::endl;
        return;
    }

    glUniformMatrix4x2fv(uniform_location, (GLsizei)mat_vec.size(), GL_FALSE, glm::value_ptr(mat_vec[0]));
}