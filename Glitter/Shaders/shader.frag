#version 430

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec4 outColor;

void main()
{
    vec3 white = vec3(1, 1, 0);  
    float roughness = 0.2;        
    float metallic = 0.8;      

    // fresnel
    vec3 viewDir = normalize(vec3(viewMatrix * modelMatrix * vec4(0, 0, 0, 1)) - gl_FragCoord.xyz);
    vec3 normal = normalize(vec3(0, 0, 1));  
    float fresnel = pow(1.0 - dot(viewDir, normal), 5.0);

    // Normal Distribution
    float roughnessSq = roughness * roughness;
    float normalDistribution = exp(-dot(normal, normal) / (roughnessSq * (dot(normal, normal) + roughnessSq)));

    // Geometry Factor
    float geometry = 1.0; 

    vec3 specular = vec3(0.04);  
    vec3 diffuse = white - (white * metallic);

    vec3 f0 = mix(vec3(0.04), white, metallic);

    vec3 kS = f0;
    vec3 kD = vec3(1.0) - kS;

    vec3 brdf = (fresnel * normalDistribution * geometry) / (4.0 * dot(normal, viewDir));

    vec3 color = (kD * diffuse + brdf * specular);

    outColor = vec4(color, 1.0);
}
