#include "shaderCommon.h"

RWTexture2D<float4> outputTexture : register(u0);

cbuffer RootConstants : register(b0)
{
    VolumetricConstants constants;
};
static const int STEP_COUNT = 512;
static const float FAR_PLANE = 10;



static const float DEG_TO_RAD = 0.01745;

static const float vFov = 70*DEG_TO_RAD;

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    static const float3 spherePos = float3(0, 0, 5);
    static const float sphereRad2 = 4.f;

    //Get screen ray
    float aspect = float(constants.screenResX) / float(constants.screenResY);

    float2 screenUV = DTid.xy / float2(constants.screenResX, constants.screenResY);
    float fovAdjust = tan(vFov / 2);
    float rayX = (2 * screenUV.x - 1)* fovAdjust * aspect;
    float rayY = (1-2 * screenUV.y) * fovAdjust;
    //Get camera mat
    float3 camRight = cross(constants.camDir, float3(0, 1, 0));
    float3 camUp = cross(camRight, constants.camDir);
    float3x3 camMat = float3x3(camRight, camUp, constants.camDir);

    float3 rayOrigin = constants.camPos;
    float3 rayDir = mul(normalize(float3(rayX, rayY, 1)),camMat);

    static const float3 SPHERE_COLOR = float3(1,0,1);
    static const float3 BACKGROUND_COLOR = float3(0, 0, 0);

    float transmittance = 1.0;

    static float stepSize = FAR_PLANE/STEP_COUNT;
    //Ray marching steps
	for (int i = 0; i < STEP_COUNT; ++i)
    {
        float3 sample = rayOrigin + rayDir * (i * stepSize);
        float3 sphereOffset = sample - spherePos;
        float distToSphere2 = dot(sphereOffset, sphereOffset);
    	if (distToSphere2 < sphereRad2) // Hit sphere
        {
            float density = (1 - sqrt(distToSphere2 / sphereRad2));

            transmittance -= density*stepSize;
        }

    }
    transmittance = saturate(transmittance);
    outputTexture[DTid.xy] = float4(lerp(SPHERE_COLOR, BACKGROUND_COLOR, transmittance), 1);


}