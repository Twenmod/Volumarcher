#include "shaderCommon.h"

RWTexture2D<float4> outputTexture : register(u0);

cbuffer RootConstants : register(b0)
{
    VolumetricConstants constants;
};
static const int STEP_COUNT = 512;
static const float FAR_PLANE = 10;


static const int2 screenResolution = int2(1920, 1080);

static const float DEG_TO_RAD = 0.01745;

static const float vFov = 90*DEG_TO_RAD;

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    static const float3 spherePos = float3(0, 0, 3);
    static const float sphereRad2 = 4.f;


    //outputTexture[DTid.xy] = float4(int2(DTid.xy) / float2(1920.f, 1080.f),0,1);

    float aspect = float(screenResolution.x) / float(screenResolution.y);
    float2 screenUV = DTid.xy / float2(screenResolution);
    float fovAdjust = tan(vFov / 2);
    float rayX = (2 * screenUV.x - 1)* fovAdjust * aspect;
    float rayY = (1-2 * screenUV.y) * fovAdjust;

    float3 camRight = cross(constants.camDir, float3(0, 1, 0));
    float3 camUp = cross(camRight, constants.camDir);
    float3x3 camMat = float3x3(camRight, camUp, constants.camDir);

    float3 rayDir = mul(normalize(float3(rayX, rayY, 1)),camMat);

    float3 rayOrigin = constants.camPos;

    static const float3 SPHERE_COLOR = float3(1,0,1);
    static const float3 BACKGROUND_COLOR = float3(0, 0, 0);

    static float stepSize = FAR_PLANE/STEP_COUNT;
    float transmittance = 1.0;
    for (int i = 0; i < STEP_COUNT; ++i)
    {
        float3 sample = rayOrigin + rayDir * (i * stepSize);
        float3 sphereOffset = sample - spherePos;
        if (dot(sphereOffset,sphereOffset) < sphereRad2) // Hit sphere
        {
            transmittance -= 0.06*stepSize;
        }

    }
    transmittance = saturate(transmittance);
    outputTexture[DTid.xy] = float4(lerp(SPHERE_COLOR, BACKGROUND_COLOR, transmittance), 1);


}