#version 330
layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 galbedospec;
layout (location = 3) out vec4 gemission;

in vec2 fragTexCoord;
in vec3 fragPos;
in vec3 fragNormal;

uniform sampler2D texture0; // diffuse
uniform sampler2D texture1; // specular
uniform sampler2D texture2; // normals
uniform sampler2D texture3; // roughness
uniform sampler2D texture4; // occlusion
uniform sampler2D texture5; // emission

out vec4 finalColor;


float linearize_depth(float depth, float n, float f)
{
    return (2*n) / (f+n-depth*(f-n));
}

void main()
{
    gnormal = normalize(fragNormal);
    gposition = fragPos;
    galbedospec.rgb = texture(texture0, fragTexCoord).rgb;
    galbedospec.a = texture(texture1, fragTexCoord).r;
    
    vec4 em = texture(texture5, fragTexCoord);
    gemission.rgba = em.rgba;
    
}
