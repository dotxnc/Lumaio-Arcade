#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
// uniform vec3 vertexNormal;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out vec3 fragPos;

// NOTE: Add here your custom variables 

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    
    // mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    // fragNormal = normalize(normalMatrix*vertexNormal);
    
    // fragPos = vec3(modelMatrix*vec4(vertexPosition, 1.0));
    // // fragPos = mvp*vec4(vertexPosition, 1.0);
    
    // gl_Position = mvp*vec4(vertexPosition, 1.0);
    
    vec4 viewpos = viewMatrix * modelMatrix * vec4(vertexPosition, 1.f);
    fragPos = viewpos.xyz;
    gl_Position = projectionMatrix * viewpos;
    
    // mat3 normalMatrix = viewMatrix * transpose(inverse(modelMatrix));
    // fragNormal = vec3(normalMatrix * vec4(vertexNormal, 0.f));
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix*modelMatrix)));
    fragNormal = normalMatrix * vertexNormal;
    
}