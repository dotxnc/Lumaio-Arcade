#version 330
layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 galbedospec;

in vec2 fragTexCoord;
in vec3 fragPos;
in vec3 fragNormal;

uniform sampler2D texture0; // diffuse
uniform sampler2D texture1; // specular
uniform sampler2D texture2; // normals

out vec4 finalColor;


float linearize_depth(float depth, float n, float f)
{
    return (2*n) / (f+n-depth*(f-n));
}

void main()
{
    gnormal = normalize(fragNormal);
    gposition = fragPos;
    // gposition.z = gl_FragCoord.z;
    // gposition.z = linearize_depth(gl_FragCoord.z, 0.01, 1000.0);
    galbedospec.rgb = texture(texture0, fragTexCoord).rgb;
    galbedospec.a = texture(texture1, fragTexCoord).r;
}
