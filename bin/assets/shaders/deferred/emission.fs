#version 330

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform sampler2D ssaobuffer;

void main()
{
    finalColor = texture(texture0, fragTexCoord) * texture(ssaobuffer, fragTexCoord);
    // finalColor.a = texture(texture0, fragTexCoord).a;
    
}
