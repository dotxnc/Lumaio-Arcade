#version 330

uniform sampler2D texture0;

in vec2 fragTexCoord;
in vec3 fragPos;
in vec2 fragUV;
out vec4 finalColor;

void main()
{
    vec2 uv = fragTexCoord;
    uv *= 1.0 - uv.xy;
    float vig = uv.x * uv.y * 500.0;
    vig = pow(vig, 100);
    vig = clamp(vig, 0.0, 1.0);
    
    finalColor = vec4(vig*texture(texture0, fragTexCoord));
    
}
