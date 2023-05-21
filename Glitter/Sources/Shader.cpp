#include "Shader.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

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
    checkCompileErrors(shaderId, "VERTEX");
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
    checkCompileErrors(m_programId, "PROGRAM");
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