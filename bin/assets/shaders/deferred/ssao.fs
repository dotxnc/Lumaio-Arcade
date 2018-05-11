#version 330

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform sampler2D normalbuffer;
uniform sampler2D depthbuffer;
uniform sampler2D noisebuffer;
uniform sampler2D positionbuffer;
uniform vec3 samples[64];

uniform mat4 projection;
const int kernelSize = 64;
const vec2 noisescale = vec2(1280.f/4.f, 720.f/4.f);

vec3 calc_crytek()
{
    const float radius = 0.5;
    const float bias = 0.025;
    
    vec3 fragPos = texture(positionbuffer, fragTexCoord).xyz;
    vec3 normal = normalize(texture(normalbuffer, fragTexCoord).rgb);
    vec3 randomVec = normalize(texture(noisebuffer, fragTexCoord * noisescale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 sample = TBN * samples[i]; // from tangent to view-space
        sample = fragPos + sample * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(positionbuffer, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / 64.f);
    
    return vec3(occlusion);
    
}

void main()
{
    vec3 color = calc_crytek();
    // vec3 color = calc_ssao();
    finalColor = vec4(color, 1.f);
    // finalColor = texture(normalbuffer, fragTexCoord);
}
