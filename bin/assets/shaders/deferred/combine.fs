#version 330

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform sampler2D bloom;
uniform float exposure = 1.f;

void main()
{
    const float gamma = 2.2f;
    vec3 hdrColor = texture(texture0, fragTexCoord).rgb;
    vec3 bloomColor = texture(bloom, fragTexCoord).rgb;
    hdrColor += bloomColor;
    vec3 result = vec3(1.0); - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.f/gamma));
    finalColor = vec4(hdrColor, 1.f);
}
