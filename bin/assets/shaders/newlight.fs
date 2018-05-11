#version 330

in vec2 fragTexCoords;
in vec3 fragPos;;
in vec3 fragNormal;

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D galbedospec;

out vec4 finalColor;

void main()
{
    vec2 uv = fragTexCoords.xy / vec2(1280, 720);
    
    vec4 c1 = texture(gposition, fragTexCoords);
    vec4 c2 = texture(gnormal, fragTexCoords);
    vec4 c3 = texture(galbedospec, fragTexCoords);
    
    finalColor = c3;
}
