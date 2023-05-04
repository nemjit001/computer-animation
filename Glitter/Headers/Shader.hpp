#ifndef SHADER
#define SHADER
#pragma once

#include <glad/glad.h>

/// <summary>
/// The Shader class can be used to register shader files for rendering,
/// as well as set those shaders as active for the render loop.
/// 
/// The Shader class manages shader memory and linking. The class follows a builder pattern to allow
/// function chaining.
/// </summary>
class Shader
{
public:
    /// <summary>
    /// Create a shader program
    /// </summary>
    Shader();
    ~Shader();

    /// <summary>
    /// Initialize the shader program so that it can be used by openGL
    /// </summary>
    void init();

    /// <summary>
    /// Clean up all memory associated with the openGL shader program
    /// </summary>
    void cleanup();

    /// <summary>
    /// Register a shader file and associated shader type to the shader program
    /// 
    /// NOTE: The shader file extension is separate from the registered shader file
    /// NOTE: function calls can be chained
    /// </summary>
    /// <param name="filepath">The file to be loaded as shader</param>
    /// <param name="shaderType">The type of shader to be loaded</param>
    /// <returns>The reference to this shader program</returns>
    Shader& registerShader(const char* filepath, GLenum shaderType);

    /// <summary>
    /// Link the registered shader into a single useable program.
    /// 
    /// NOTE: function calls can be chained
    /// </summary>
    /// <returns>The reference to this shader program</returns>
    Shader& link();

    /// <summary>
    /// Use the shader program to render with
    /// 
    /// NOTE: function calls can be chained
    /// </summary>
    /// <returns>The reference to this shader program</returns>
    Shader& use();

private:
    GLuint m_programId;
};

#endif // SHADER
