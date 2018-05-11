#version 330
layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 galbedospec;

in vec2 fragTexCoords;
in vec3 fragPos;;
in vec3 fragNormal;

uniform sampler2D texture0;

void main()
{
    gposition = fragPos;
    gnormal = fragNormal;
    galbedospec = texture(texture0, fragTexCoords);
}
