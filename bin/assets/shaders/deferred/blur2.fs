#version 330

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec2 direction = vec2(1, 0);
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
  vec2 off1 = vec2(1.411764705882353) * direction;
  vec2 off2 = vec2(3.2941176470588234) * direction;
  vec2 off3 = vec2(5.176470588235294) * direction;
  color += texture2D(image, uv) * 0.1964825501511404;
  color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
  color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
  return color;
}

void main()
{
    // vec2 texelsize = 1.f / vec2(1280, 720);
    // vec4 result;
    // for (int x = -2; x < 2; x++) {
    //     for (int y = -2; y < 2; y++) {
    //         vec2 offset = vec2(float(x), float(y)) * texelsize;
    //         result += texture(texture0, fragTexCoord+offset);
    //     }
    // }
    // result = blur13(texture0, fragTexCoord, vec2(1280, 720), direction);
    // finalColor = result;//vec4(result / (4.f*4.f));
    // finalColor.a *= texture(texture0, fragTexCoord).r;
    
    vec2 tex_offset = 1.0 / textureSize(texture0, 0); // gets size of single texel
    vec3 result = texture(texture0, fragTexCoord).rgb * weight[0]; // current fragment's contribution
    if(direction.x == 1)
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(texture0, fragTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i%5];
            result += texture(texture0, fragTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i%5];
        }
    }
    else
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(texture0, fragTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i%5];
            result += texture(texture0, fragTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i%5];
        }
    }
    finalColor = vec4(result, 1.0);
}
