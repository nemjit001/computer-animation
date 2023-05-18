#version 430

layout(location = 0) in vec3 vPos;
layout(location = 1) uniform mat4 projectionMatrix;

void main()
{
    // We need to add a 4th value before our vector can be transformed by a 4x4 matrix.
    vec4 withW = vec4(vPos, 1.0);


    // Important:
    // Projection matrix calculations are not final / verified. They are just here as initial version, to show that the matrix is at least being passed to the GPU.
    vec4 transformed = projectionMatrix * withW;

    gl_Position = vec4(transformed.xyz, 1.0);
}
