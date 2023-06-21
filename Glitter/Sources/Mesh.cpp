#include "Mesh.hpp"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#define STB_IMAGE_IMPLEMENTATION
//#define SCENE_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords | aiProcess_FindInvalidData | aiProcess_TransformUVCoords | aiProcess_PreTransformVertices)
#define SCENE_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals |  aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords | aiProcess_FindInvalidData | aiProcess_TransformUVCoords)

// System Headers
#include <stb_image.h>
#include <Timer.hpp>

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
    //Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filename,
        SCENE_LOAD_FLAGS
    );

    if (!scene)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    else
    {
        dir = filename.substr(0, filename.find_last_of('/'));
        Parse(scene->mRootNode, scene);

        // Set root node
        this->scene = scene;

        // Set Inverse Transform Matrix (not needed if we stick to single mesh models)
        inverse_transform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));

        // Parse animations
        ParseAnimations(scene);
    }
}

Mesh::Mesh(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indices, std::vector<Texture> const& textures, const Shader shader)
    :
    m_vertices(verts),
    m_indices(indices),
    m_textures(textures),
    shader(shader)

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
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


    glVertexAttribPointer(6, MAXIMUM_BONES, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);  // Bone weights
    glBindVertexArray(0);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::Render(glm::mat4 view, glm::mat4 model, glm::mat4 projection, glm::vec3 cam_pos, glm::vec3 light_pos, glm::vec3 base_color, glm::vec3 manual_light_color, float manual_metallic, float manual_roughness, GLuint texture_diffuse, GLuint texture_normal, GLuint texture_specular)
{
    // Use shader
    shader.use();

    // Pass uniforms
    shader.setMat4("viewMatrix", view);
    shader.setMat4("modelMatrix", model);
    shader.setMat4("projectionMatrix", projection);
    shader.setVec3("CamPos", cam_pos);
    shader.setVec3("LightPosition", light_pos);
    shader.setVec3("BaseColor", base_color);
    shader.setVec3("ManualLightColor", manual_light_color);
    shader.setFloat("ManualMetallic", manual_metallic);
    shader.setFloat("ManualRoughness", manual_roughness);
    shader.setInt("DiffuseTexture", 0);
    shader.setInt("NormalTexture", 1);
    shader.setInt("SpecularTexture", 2);


    for (auto& mesh : m_subMeshes)
        mesh->Render(view, model, projection, cam_pos, light_pos, base_color, manual_light_color, manual_metallic, manual_roughness, texture_diffuse, texture_normal, texture_specular);

    // bind appropriate textures
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
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
    glBindVertexArray(0);
}

void Mesh::ParseAnimations(const aiScene* scene)
{
    // Check for animations
    if (scene->HasAnimations())
    {
        unsigned int n_animations = scene->mNumAnimations;
        std::cout << "Found " << n_animations << " animations in Scene!" << std::endl;

        // Retrieve animations and parse them to AnimationClip objects
        for (unsigned int i = 0; i < n_animations; i++)
        {
            aiAnimation* current_animation = scene->mAnimations[i];

            // TODO: Currently considers same size of all channels (realistic?)
            // Parse channels
            int max_frames = 0;
            std::map<std::string, AnimationPose> poses;
            for (unsigned int j = 0; j < current_animation->mNumChannels; j++)
            {
                aiNodeAnim* current_channel = current_animation->mChannels[j];

                std::string channel_bone_name = std::string(current_channel->mNodeName.data);           // Get name of affected node

                // Check keyframe number
                if (current_channel->mNumPositionKeys > max_frames)
                    max_frames = current_channel->mNumPositionKeys;

                // Parse SQTs of channel
                std::vector<SQT> sqts;
                for (unsigned int k = 0; k < current_channel->mNumPositionKeys; k++)
                {
                    SQT new_sqt;
                    new_sqt.time = current_channel->mPositionKeys->mTime;
                    new_sqt.scale = ConvertVector3DToGLMFormat(current_channel->mScalingKeys[k].mValue);
                    new_sqt.translation = ConvertVector3DToGLMFormat(current_channel->mPositionKeys[k].mValue);
                    new_sqt.rotation = ConvertQuaternionToGLMFormat(current_channel->mRotationKeys[k].mValue);

                    sqts.push_back(new_sqt);
                }

                AnimationPose new_pose;
                new_pose.bone_name = channel_bone_name;
                new_pose.bonePoses = sqts;

                // Add AnimationPose to map
                poses.insert({channel_bone_name, new_pose});
            }

            AnimationClip new_animation_clip = AnimationClip(std::string(current_animation->mName.data), this->m_bones.size(), max_frames, current_animation->mDuration, current_animation->mTicksPerSecond, poses);
            m_animations.push_back(new_animation_clip);
        }
    }
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
    // Parse vertices
    std::vector<Vertex> vertices;
    Vertex vert;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vert = {};
        vert.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals())
            vert.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
        {
            vert.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            if (mesh->mTangents)
            {
                vert.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vert.biTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
            else
            {
                vert.tangent = glm::vec3(1.0f);
                vert.biTangent = glm::vec3(1.0f);
            }
        }
        else
            vert.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vert);
    }

    // Parse textures
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> textures;
    std::vector<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> normalMaps = LoadMaterialTextures(mat, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<Texture> heightMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // Parse indices
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    this->m_vertices = vertices;
    this->m_indices = indices;
    this->m_textures = textures;

    // Parse bones
    ExtractBoneWeightForVertices(this->m_vertices, mesh, scene);

    m_subMeshes.push_back(
        std::unique_ptr<Mesh>(new Mesh(this->m_vertices, this->m_indices, this->m_textures, shader))
    );
}

void Mesh::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, const aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = bone_map;
    int& boneCount = m_boneCounter;

    std::cout << "Found " << mesh->mNumBones << " Bones in Mesh " << mesh->mName.C_Str() << std::endl;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        int boneId = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offsetMatrix = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            //boneInfoMap[boneName] = boneCount;
            boneInfoMap.insert({ boneName, boneCount });
            m_bones.push_back(newBoneInfo);
            boneId = boneCount;
            boneCount++;
        }
        else
        {
            //boneId = boneInfoMap[boneName];
            boneId = boneInfoMap.find(boneName)->second;
        }
        assert(boneId != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
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
    unsigned int bone_number = vertex.bone_num;                     // The array index where the bone will be added

    if (bone_number > 3)
    {
        std::cout << "ERROR::Exceeded MAX_BONES!" << std::endl;

        return;
    }

    // Add Bone ID and Weight
    vertex.boneIDs[bone_number] = boneId;
    vertex.weights[bone_number] = weight;

    // Increment Bone Number in vertex
    vertex.bone_num++;
}

inline glm::mat4 Mesh::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    return glm::transpose(glm::make_mat4(&from.a1));
}

// Converts aiVector3D to GLM::vec3
inline glm::vec3 Mesh::ConvertVector3DToGLMFormat(const aiVector3D& src)
{
    return glm::vec3(src.x, src.y, src.z);
}

// Converts aiQuaternion to GLM::quat
inline glm::quat Mesh::ConvertQuaternionToGLMFormat(const aiQuaternion& src)
{
    return glm::quat(src.w, src.x, src.y, src.z);
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

        if (format == GL_RGBA)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
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

void Mesh::Animate(int frame)
{
    // TODO: Switching between animations can be added!

    std::vector<glm::mat4> bone_transforms;                     // Vector to be passed to vertex shader, containing all bone transforms

    glm::mat4 initial_matrix = glm::mat4(1.0f);

    // Traverse nodes from root node
    TraverseNode(frame, scene->mRootNode, initial_matrix);

    bone_transforms.resize(m_boneCounter);

    // Traverse updated bones
    for (int i = 0; i < m_boneCounter; i++)
    {
        bone_transforms[i] = m_bones[i].bone_transform;
    }

    shader.use();

    // Write bone transforms to vertex shader
    shader.setMat4Vector("boneTransforms", bone_transforms);
}

void Mesh::AnimateLI(double m_currentTime)
{
    // TODO: Switching between animations can be added!

    std::vector<glm::mat4> bone_transforms;                     // Vector to be passed to vertex shader, containing all bone transforms

    glm::mat4 initial_matrix = glm::mat4(1.0f);

    // Traverse nodes from root node
    TraverseNode(m_currentTime, scene->mRootNode, initial_matrix);

    bone_transforms.resize(m_boneCounter);

    // Traverse updated bones
    for (int i = 0; i < m_boneCounter; i++)
    {
        bone_transforms[i] = m_bones[i].bone_transform;
    }

    shader.use();

    // Write bone transforms to vertex shader
    shader.setMat4Vector("boneTransforms", bone_transforms);
}

void Mesh::TraverseNode(const double m_currentTime, const aiNode* node, const glm::mat4& parent_transform)
{
    std::string node_name(std::string(node->mName.data));
    glm::mat4 node_transform = ConvertMatrixToGLMFormat(node->mTransformation);

    // Get SQT
    SQT sqt;
    auto sqt_it = m_animations.back().poseSamples.find(node_name);
    if (sqt_it != m_animations.back().poseSamples.end())
    {
        const std::vector<SQT>& bonePoses = sqt_it->second.bonePoses;
        const int numFrames = static_cast<int>(bonePoses.size());

        // Check if keyframes exist
        if (numFrames > 0)
        {
            // Find frames
            int frameIndex = static_cast<int>(m_currentTime) % numFrames;
            int nextFrameIndex = (frameIndex + 1) % numFrames;

            const SQT& currentFrameSQT = bonePoses[frameIndex];
            const SQT& nextFrameSQT = bonePoses[nextFrameIndex];

            // Calculate the interpolation factor
            double t = m_currentTime - static_cast<int>(m_currentTime);

            // Interpolate scale
            glm::vec3 scale = glm::mix(currentFrameSQT.scale, nextFrameSQT.scale, static_cast<float>(t));

            // Interpolate rotation
            glm::quat rotation = glm::slerp(currentFrameSQT.rotation, nextFrameSQT.rotation, static_cast<float>(t));

            // Interpolate translation
            glm::vec3 translation = glm::mix(currentFrameSQT.translation, nextFrameSQT.translation, static_cast<float>(t));

            glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);
            glm::mat4 rotation_matrix = glm::toMat4(rotation);
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);

            node_transform = translation_matrix * rotation_matrix * scale_matrix;
        }
    }

    // Combine with parent
    glm::mat4 global_transformation = parent_transform * node_transform;

    // Get Bone
    auto bone_it = bone_map.find(node_name);
    if (bone_it != bone_map.end())
    {
        m_bones[bone_it->second].bone_transform = inverse_transform * global_transformation * m_bones[bone_it->second].offsetMatrix;
    }

    // Recursion to traverse all nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        TraverseNode(m_currentTime, node->mChildren[i], global_transformation);
    }
}


Shader Mesh::getShader()
{
    return shader;
}

int Mesh::GetAnimationFrameNum()
{
    return m_animations.back().GetFrameNum();
}

bool Mesh::HasAnimations()
{
    if (m_animations.empty())
        return false;
    else
        return true;
}