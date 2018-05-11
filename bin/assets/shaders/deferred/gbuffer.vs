#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out vec3 fragPos;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    vec4 viewpos = viewMatrix * modelMatrix * vec4(vertexPosition, 1.f);
    fragPos = viewpos.xyz;
    gl_Position = projectionMatrix * viewpos;
    
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix*modelMatrix)));
    fragNormal = normalMatrix * vertexNormal;
    
}