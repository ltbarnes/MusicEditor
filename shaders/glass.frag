#version 330

in vec3 vertex;                 // The position of the vertex, in eye space!
in vec3 vertexToEye;            // Vector from the vertex to the eye
in vec3 eyeNormal;		// Normal of the vertex, in eye space!

uniform float r0;		// The R0 value to use in Schlick's approximation
uniform float eta1D;		// The eta value to use initially
uniform vec3  eta;		// Contains one eta for each channel (use eta.r, eta.g, eta.b in your code)

uniform mat4 view;
uniform mat4 model;

uniform samplerCube envMap;

out vec4 fragColor;

void main()
{
    vec3 n = normalize(eyeNormal);
    vec3 eyeToVertex = normalize(vertex); //remember we are in eye space!\
    // TODO: fill the rest in
    
    fragColor = vec4(0.0);    
    float F = r0 + (1.0 - r0) * pow(1.0 - dot(n, normalize(-eyeToVertex)), 5.0);
    
    vec4 reflectVec = inverse(view * model) * vec4(reflect(eyeToVertex, n), 0.0);
    vec4 refractVecR = inverse(view * model) * vec4(refract(eyeToVertex, n, eta.r), 0.0);
    vec4 refractVecG = inverse(view * model) * vec4(refract(eyeToVertex, n, eta.g), 0.0);
    vec4 refractVecB = inverse(view * model) * vec4(refract(eyeToVertex, n, eta.b), 0.0);
    
    vec4 reflectColor = texture(envMap, normalize(reflectVec).xyz);
    vec4 refractColor = vec4(1.0);
    refractColor.r = texture(envMap, normalize(refractVecR).xyz).r;
    refractColor.g = texture(envMap, normalize(refractVecG).xyz).g;
    refractColor.b = texture(envMap, normalize(refractVecB).xyz).b;
    
//    fragColor = mix(refractColor, reflectColor, F);
    fragColor = vec4(1);
}
