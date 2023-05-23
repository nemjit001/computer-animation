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
        parse(scene->mRootNode, scene);
    }
}

Mesh::Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices)
    :
    m_vertices(verts),
    m_indices(indices)
{
    // bind the default vertex array object
    glGenBuffers(1, &m_VertexArrayObject);
    glBindVertexArray(m_VertexArrayObject);
    
    // bind & create the vertex buffer
    glGenBuffers(1, &m_VertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(Vertex),
        verts.data(),
        GL_STATIC_DRAW
    );

    // bind & create the index buffer
    glGenBuffers(1, &m_IndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // Set Shader Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);   // Vertex Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);   // Normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);   // Texture Coordinates

    glBindVertexArray(0);
    glDeleteBuffers(1, &m_VertexBufferObject);
    glDeleteBuffers(1, &m_IndexBufferObject);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
}

void Mesh::Render()
{
    for (auto& mesh : m_subMeshes)
        mesh->Render();

    glBindVertexArray(m_VertexArrayObject);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::parse(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        parse(scene->mMeshes[node->mMeshes[i]], scene);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        parse(node->mChildren[i], scene);
}

void Mesh::parse(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    Vertex vert;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vert = {};
        vert.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vert.normal   = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);

        // Get Texture Coordinates from UV Channel 0
        assert(mesh->mTextureCoords[0]);
        vert.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vertices.push_back(vert);
    }

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    m_subMeshes.push_back(
        std::unique_ptr<Mesh>(new Mesh(vertices, indices))
    );
}
