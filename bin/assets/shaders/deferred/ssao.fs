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
const float factor = 1.f / 64.f;

float saturate( in float a ) { return clamp( a, 0.0, 1.0 ); }
vec2  saturate( in vec2 a )  { return clamp( a, 0.0, 1.0 ); }
vec3  saturate( in vec3 a )  { return clamp( a, 0.0, 1.0 ); }
vec4  saturate( in vec4 a )  { return clamp( a, 0.0, 1.0 ); }

vec3 depthnormal(float depth)
{
    const vec2 offset1 = vec2(0.0f,0.001f);
    const vec2 offset2 = vec2(0.001f,0.0f);
    
    float depth1 = texture(depthbuffer, fragTexCoord+offset1).r;
    float depth2 = texture(depthbuffer, fragTexCoord+offset2).r;
    
    vec3 p1 = vec3(offset1, depth1 - depth);
    vec3 p2 = vec3(offset2, depth2 - depth);
    
    vec3 normal = cross(p1,p2);
    normal.z = -normal.z;
    
    return normalize(normal);
}

float linearize_depth(float depth, float n, float f)
{
    return (2*n) / (f+n-depth*(f-n));
}

vec3 calc_ssao()
{
    const float radius = 0.2f;
    const float base = 0.2f;
    const float strength = 1.f;
    const float area = 0.0075f;
    const float falloff = 0.000001f;
    
    vec3 Normal = normalize(texture(normalbuffer, fragTexCoord)).rgb;
    Normal = normalize(Normal * 2.0 - 1.0);
    vec3 FragPos = (texture(positionbuffer, fragTexCoord)).rgb;
    // return Normal;
    
    vec3 randomVec = normalize(texture(noisebuffer, fragTexCoord*noisescale).rgb).xyz;
    
    float depth = texture(depthbuffer, fragTexCoord).r;
    vec3 position = vec3(fragTexCoord, depth);//VSPositionFromDepth(fragTexCoord);
    
    float radius_depth = radius/depth;
    float occlusion = 0.f;
    for (int i = 0; i < 64; i++)
    {
        vec3 ray = radius_depth * reflect(samples[i], randomVec);
        vec3 hemi_ray = position + sign(dot(ray,Normal))*ray;
        float occ_depth = texture(depthbuffer, saturate(hemi_ray.xy)).r;
        float difference = depth - occ_depth;
        occlusion += step(falloff, difference) * (1.f-smoothstep(falloff, area, difference));
    }
    float ao = 1.f - strength * occlusion * factor;
    occlusion = saturate(ao + base);
    
    return vec3(occlusion);
}

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
