#include "Mesh.hpp"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION

// System Headers
#include <stb_image.h>

Mesh::Mesh()
    :
    m_VBO(0),
    m_VAO(0)
{
    glGenVertexArrays(1, &m_VAO);
}

Mesh::Mesh(std::string const& filename, const Shader& shader)
    :
    Mesh()
{
    this->shader = shader;
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
        dir = filename.substr(0, filename.find_last_of('/'));
        Parse(scene->mRootNode, scene);
    }
}

Mesh::Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices, std::vector<Texture> const& textures)
    :
    m_vertices(verts),
    m_indices(indices),
    m_textures(textures)
{
    // bind the default vertex array object
    glGenBuffers(1, &m_VAO);
    glBindVertexArray(m_VAO);
    
    // bind & create the vertex buffer
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(Vertex),
        verts.data(),
        GL_STATIC_DRAW
    );

    // bind & create the index buffer
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // Set Shader Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, position));
    glEnableVertexAttribArray(0); // Vertex Positions

    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1); // Vertex Normals
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);  // Vertex texture coords
    
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);     // Vertex tangent

    
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));
    glEnableVertexAttribArray(4);  // Vertex bitangent
   
    glVertexAttribIPointer(5, MAXIMUM_BONES, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);   // Bone ids

    
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);  // Bone weights
    glBindVertexArray(0);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::Render()
{
    for (auto& mesh : m_subMeshes)
        mesh->Render();

    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.getShaderID(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);

}

void Mesh::Parse(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        Parse(scene->mMeshes[node->mMeshes[i]], scene);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        Parse(node->mChildren[i], scene);
}

void Mesh::Parse(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    Vertex vert;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vert = {};
        vert.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if(mesh->HasNormals())
            vert.normal   = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
        {
            vert.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            vert.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vert.biTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        else
            vert.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vert);
    }

    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> textures;
    std::vector<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> normalMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<Texture> heightMaps = LoadMaterialTextures(mat, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    m_subMeshes.push_back(
        std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures))
    );
}

void Mesh::SetBoneToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAXIMUM_BONES; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void Mesh::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = m_bones;
    int& boneCount = m_boneCounter;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        int boneId = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offsetMatrix = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneId = boneCount;
            boneCount++;
        }
        else
        {
            boneId = boneInfoMap[boneName].id;
        }
        assert(boneId != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneId, weight);
        }
    }
}

void Mesh::SetVertexBoneData(Vertex& vertex, int boneId, float weight)
{
    for (int i = 0; i < MAXIMUM_BONES; ++i)
    {
        if (vertex.boneIDs[i] < 0)
        {
            vertex.boneIDs[i] = boneId;
            vertex.weights[i] = weight;
            break;
        }
    }
}

inline glm::mat4 Mesh::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

std::vector<Texture> Mesh::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < m_textures.size(); j++)
        {
            if (std::strcmp(m_textures[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(m_textures[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->dir);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            m_textures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}


unsigned int Mesh::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}