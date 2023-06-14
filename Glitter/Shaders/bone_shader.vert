#version 430
// *****************************************************
// Shader that implements Linear Skinning
// *****************************************************

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 5) in ivec4 boneIDs;      // Size of 4 is in accordance with the 4 bone per vertex convention
layout(location = 6) in vec4 boneWeights;

const int MAX_BONES = 80;                   // We need a maximum number, and 80 should be safe for the vast majority of rigs

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;

uniform mat4 boneTransforms[MAX_BONES];
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 new_position;

    // Loop between 4 bones for position
    mat4 finalBoneTransform = boneTransforms[boneIDs[0]] * boneWeights[0];
    finalBoneTransform += boneTransforms[boneIDs[1]] * boneWeights[1];
    finalBoneTransform += boneTransforms[boneIDs[2]] * boneWeights[2];
    finalBoneTransform += boneTransforms[boneIDs[3]] * boneWeights[3];

    // Calculate final vertex position
    new_position = finalBoneTransform * vec4(position, 1.0);

    // Calculate final normal direction
    vec4 new_normal = finalBoneTransform * vec4(normal, 0.0);

    // TODO: Tangent and Bitangent will also be affected by finalBoneTransform!

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * new_position;
    Normal = mat3(transpose(inverse(modelMatrix))) * new_normal.xyz;    // Presumably correct
    WorldPos = vec3(modelMatrix * new_position);                // Presumably correct
    TexCoords = texCoords;                                      // Just passed to the Fragment Shader
}