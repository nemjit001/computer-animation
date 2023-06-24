#version 430
// **************************************************************
// Shader that implements Dual Quaternion Skinning without scale
// **************************************************************

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(location = 5) in ivec4 boneIDs;      // Size of 4 is in accordance with the 4 bone per vertex convention
layout(location = 6) in vec4 boneWeights;

const int MAX_BONES = 120;                   // We need a maximum number, and 120 should be safe for the vast majority of rigs

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;
out vec3 TestColor;

uniform mat4x2 boneTransforms[MAX_BONES];   // Each bone transform is represented by two quaternions (mat4x2)
//uniform mat4 scaleTransforms[MAX_BONES];    // Scaling for each bone
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

// Function to handle quaternion multiplication in GLSL
vec4 multiplyQuat(vec4 q1, vec4 q2)
{
    vec4 q_prod;

    q_prod.x = q1.x * q2.x - q1.y * q2.y - q1.z * q2.z - q1.w * q2.w;
    q_prod.x = q1.x * q2.y + q1.y * q2.x + q1.z * q2.w - q1.w * q2.z;
    q_prod.x = q1.x * q2.x - q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    q_prod.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;

    return q_prod;
}

// Function to convert dual quaternion to matrix
mat4x3 DQToMatrix(vec4 Qn, vec4 Qd)
{	
	mat4x3 M;
	float len2 = dot(Qn, Qn);
	float w = Qn.x, x = Qn.y, y = Qn.z, z = Qn.w;
	float t0 = Qd.x, t1 = Qd.y, t2 = Qd.z, t3 = Qd.w;
		
	M[0][0] = w*w + x*x - y*y - z*z; M[1][0] = 2*x*y - 2*w*z; M[2][0] = 2*x*z + 2*w*y;
	M[0][1] = 2*x*y + 2*w*z; M[1][1] = w*w + y*y - x*x - z*z; M[2][1] = 2*y*z - 2*w*x; 
	M[0][2] = 2*x*z - 2*w*y; M[1][2] = 2*y*z + 2*w*x; M[2][2] = w*w + z*z - x*x - y*y;
	
	M[3][0] = -2*t0*x + 2*w*t1 - 2*t2*z + 2*y*t3;
	M[3][1] = -2*t0*y + 2*t1*z - 2*x*t3 + 2*w*t2;
	M[3][2] = -2*t0*z + 2*x*t2 + 2*w*t3 - 2*t1*y;
	
	M /= len2;
	
	return M;	
}

void main()
{
    vec4 new_position;

    mat4x2 dq0 = boneTransforms[boneIDs.x];
	mat4x2 dq1 = boneTransforms[boneIDs.y];
	mat4x2 dq2 = boneTransforms[boneIDs.z];
	mat4x2 dq3 = boneTransforms[boneIDs.w];

    // Increases DQS robustness
    if (dot(dq0[0], dq1[0]) < 0.0) dq1 *= -1.0;
	if (dot(dq0[0], dq2[0]) < 0.0) dq2 *= -1.0;	
	if (dot(dq0[0], dq3[0]) < 0.0) dq3 *= -1.0;

    // Loop between 4 bones for position
    mat4x2 finalBoneTransform = dq0 * boneWeights.x;
    finalBoneTransform += dq1 * boneWeights.y;
    finalBoneTransform += dq2 * boneWeights.z;
    finalBoneTransform += dq3 * boneWeights.w;

//    mat4 finalScale = scaleTransforms[boneIDs.x] * boneWeights.x;
//    finalScale += scaleTransforms[boneIDs.y] * boneWeights.y;
//    finalScale += scaleTransforms[boneIDs.z] * boneWeights.z;
//    finalScale += scaleTransforms[boneIDs.w] * boneWeights.w;

    // Separate into Translation and Rotation Quaternions (stupid column-major GLSL)
    vec4 rotation_quat = vec4(finalBoneTransform[0][0], finalBoneTransform[1][0], finalBoneTransform[2][0], finalBoneTransform[3][0]);
    vec4 translation_quat = vec4(finalBoneTransform[0][1], finalBoneTransform[1][1], finalBoneTransform[2][1], finalBoneTransform[3][1]);

    mat4x3 qt = DQToMatrix(rotation_quat, translation_quat);

    // Apply scale
//    new_position = finalScale * vec4(position, 1.0);

    // Calculate final vertex position
//    new_position = vec4(qt * new_position, 1.0);
    new_position = vec4(qt * vec4(position, 1.0), 1.0);

    // Calculate final normal direction
    vec3 new_normal = qt * vec4(normal, 0.0);

    // TODO: Tangent and Bitangent will also be affected by finalBoneTransform!

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * new_position;
    Normal = mat3(transpose(inverse(modelMatrix))) * new_normal;    // Presumably correct
    WorldPos = vec3(modelMatrix * new_position);                // Presumably correct
    TexCoords = texCoords;                                      // Just passed to the Fragment Shader
}