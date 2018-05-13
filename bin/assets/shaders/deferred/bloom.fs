#version 330

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D texture0;

float luma(vec4 color) {
  return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}

void main()
{
    vec4 c = texture(texture0, fragTexCoord);
    if (luma(c*c.a) > 0.3) {
        finalColor = vec4(c.rgb, c.a);
    } else {
        finalColor = vec4(0, 0, 0, 0);
    }
    
    // finalColor = texture(texture0, fragTexCoord);
}
