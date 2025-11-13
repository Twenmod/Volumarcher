RWTexture2D<float4> outputTexture : register(u0);

static const int STEP_COUNT = 128;
static const float FAR_PLANE = 5;


static const int2 screenResolution = int2(1920, 1080);

static const float DEG_TO_RAD = 0.01745;

static const float vFov = 90*DEG_TO_RAD;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    static const float3 spherePos = float3(0, 0, 3);
    static const float sphereRad = 2.f;


    //outputTexture[DTid.xy] = float4(int2(DTid.xy) / float2(1920.f, 1080.f),0,1);

    float aspect = float(screenResolution.x) / float(screenResolution.y);
    float2 screenUV = DTid.xy / float2(screenResolution);
    float fovAdjust = tan(vFov / 2);
    float rayX = (2 * screenUV.x - 1)* fovAdjust * aspect;
    float rayY = (1-2 * screenUV.y) * fovAdjust;

    float3 rayDir = normalize(float3(rayX, rayY, 1));
    float3 rayOrigin = float3(0,0,0);

    float3 color = float3(0,0,0);

    static float stepSize = FAR_PLANE/STEP_COUNT;
    for (int i = 0; i < STEP_COUNT; ++i)
    {
        float3 sample = rayOrigin + rayDir * (i * stepSize);
        //color += length(sample - spherePos);
        if (length(sample - spherePos) < sphereRad) // Hit sphere
        {
            color += float3(0.2,0.2,0.2);
        }

    }
    outputTexture[DTid.xy] = float4(color, 1);


}