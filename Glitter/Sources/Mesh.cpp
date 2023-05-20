#include "Mesh.hpp"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION

// System Headers
#include <stb_image.h>

Mesh::Mesh()
    :
    m_VertexBufferObject(0),
    m_VertexArrayObject(0)
{
    glGenVertexArrays(1, &m_VertexArrayObject);
}

Mesh::Mesh(std::string const& filename)
    :
    Mesh()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcessPreset_TargetRealtime_MaxQuality |
        aiProcess_OptimizeGraph                   |
        aiProcess_FlipUVs
    );

    if (!scene)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    else
    {
        // parse scene
    }
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
}

void Mesh::Render()
{
    //
}