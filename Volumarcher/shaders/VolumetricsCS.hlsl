#include "ShaderCommon.h"

static const float PI = 3.14159;

RWTexture2D<float4> outputTexture : register(u0);
Texture2D<float> sceneDepth : register(t0);

cbuffer RootConstants : register(b0)
{
    VolumetricConstants constants;
};

StructuredBuffer<Volume> volumes : register(t1);

Texture3D<float> billowNoise : register(t2);
SamplerState noiseSampler : register(s0);

static const int STEP_COUNT = 128; // Step count for main ray
static const int DIRECT_STEP_COUNT = 16; // Steps for getting direct lighting
static const int AMBIENT_STEP_COUNT = 4; // Steps for getting summed ambient density
static const float FAR_PLANE = 6;


//TODO: Not hardcode this
static const float3 SUN_DIR = normalize(float3(0.4, -1, 0.4));
static const float3 SUN_LIGHT = float3(0.996, 0.9, 0.8) * 20;

static const float3 BACKGROUND_COLOR_UP = float3(0.467, 0.529, 0.671);
static const float3 BACKGROUND_COLOR_DOWN = float3(0.694, 0.596, 0.467)*0.5;
static const float3 AMBIENT_COLOR = float3(0.467, 0.529, 0.671);


static const float ABSORPTION_SCATTERING = 1.0;
static const float ECCENTRICITY = 0.2;


static const float DEG_TO_RAD = 0.01745;

static const float vFov = 70 * DEG_TO_RAD;

//Remap from https://stackoverflow.com/a/3451607
float Remap(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

//Phase function from https://www.guerrilla-games.com/read/synthesizing-realistic-clouds-for-video-games
float HenyeyGreensteinPhase(float inCosAngle, float inG)
{
    float num = 1.0 - inG * inG;
    float denom = 1.0 + inG * inG - 2.0 * inG * inCosAngle;
    float rsqrt_denom = rsqrt(denom);
    return num * rsqrt_denom * rsqrt_denom * rsqrt_denom * (1.0 / (4.0 * PI));
}

//Sample dimensional profile (base density)
float SampleProfile(int _volumeId, float3 _sample, float _distToVolume2 /*TODO shaped: Remove after implementing*/)
{
	//Sphere
    float profile = volumes[_volumeId].baseDensity * (1 - _distToVolume2 / sqrt(volumes[_volumeId].squaredRad));
    return profile;
}

float SampleDensity(float3 _sample, float _profile)
{
    float density = _profile;
	//Magic numbers to make texture tiling less obvious
    float3 noiseTexSample = (float3(_sample) + float3(14.34, 14.34, 14.34)) * 0.23;
    float noise = saturate(billowNoise.SampleLevel(noiseSampler, noiseTexSample, 0));
    density = saturate(Remap(density, noise
               , 1, 0, 1));

    return density;
}

float GetSummedAmbientDensity(float3 _sample)
{
    //TODO shaped: Bad step size that assumes 1 sphere
    float stepSize = sqrt(volumes[0].squaredRad) / AMBIENT_STEP_COUNT;
    float density = 0;
    for (int i = 0; i < AMBIENT_STEP_COUNT; ++i)
    {
        float3 sample = _sample + float3(0, 1, 0) * stepSize * i;
        for (int volumeId = 0; volumeId < VOLUME_AMOUNT; ++volumeId)
        {
            float3 sphereOffset = sample - volumes[volumeId].position;
            float distToSphere2 = dot(sphereOffset, sphereOffset);
            if (distToSphere2 < volumes[volumeId].squaredRad) // Hit sphere
            {
                density += SampleDensity(sample, SampleProfile(volumeId, sample, distToSphere2));
            }
        }
    }
    return density;
}




float3 GetDirectLighting(float3 _sample)
{

    float transmittance = 1.0;
    float stepSize = FAR_PLANE*0.5 / DIRECT_STEP_COUNT;
    for (int i = 0; i < AMBIENT_STEP_COUNT; ++i)
    {
        float3 sample = _sample + -SUN_DIR * (i * stepSize);
        for (int volumeId = 0; volumeId < VOLUME_AMOUNT; ++volumeId)
        {
            float3 sphereOffset = sample - volumes[volumeId].position;
            float distToSphere2 = dot(sphereOffset, sphereOffset);
            if (distToSphere2 < volumes[volumeId].squaredRad) // Hit sphere
            {
                float profile = SampleProfile(volumeId, sample, distToSphere2);
                float density = SampleDensity(sample, profile);
                transmittance = saturate(transmittance * exp(-stepSize * ABSORPTION_SCATTERING * density));
            }
        }
    }
    return SUN_LIGHT * transmittance;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 screenUV = DTid.xy / float2(constants.screenResX, constants.screenResY);
    float screenDepth = sceneDepth.SampleLevel(noiseSampler, screenUV, 0);
    if (screenDepth > 0)
        return;

	//Get screen ray
    float aspect = float(constants.screenResX) / float(constants.screenResY);

    float fovAdjust = tan(vFov / 2);
    float rayX = (2 * screenUV.x - 1) * fovAdjust * aspect;
    float rayY = (1 - 2 * screenUV.y) * fovAdjust;
    //Get camera mat
    float3 camRight = normalize(cross(constants.camDir, float3(0, 1, 0)));
    float3 camUp = cross(camRight, constants.camDir);
    float3x3 camMat = float3x3(camRight, camUp, constants.camDir);

    float3 rayOrigin = constants.camPos;
    float3 rayDir = mul(normalize(float3(rayX, rayY, 1)), camMat);


    float3 background = lerp(BACKGROUND_COLOR_DOWN, BACKGROUND_COLOR_UP, saturate((rayDir.y * 0.5) + 0.55));


    float transmittance = 1.0;

    static float stepSize = FAR_PLANE / STEP_COUNT;
    float3 light = 0;

    //Ray marching steps
    for (int i = 0; i < STEP_COUNT; ++i)
    {
        float3 sample = rayOrigin + rayDir * (i * stepSize);

        float density = 0;
        float3 ambientLight = 0;
        for (int volumeId = 0; volumeId < VOLUME_AMOUNT; ++volumeId)
        {
            float3 sphereOffset = sample - volumes[volumeId].position;
            float distToSphere2 = dot(sphereOffset, sphereOffset);
            if (distToSphere2 < volumes[volumeId].squaredRad) // Hit sphere
            {
                float profile = SampleProfile(volumeId, sample, distToSphere2);
                density += SampleDensity(sample, profile);

                ambientLight += saturate((1 - profile) * exp(-GetSummedAmbientDensity(sample))) * (AMBIENT_COLOR * PI);
            }
        }
        float lightAngle = dot(rayDir, -SUN_DIR);
        float3 directLight = GetDirectLighting(sample)*HenyeyGreensteinPhase(lightAngle,ECCENTRICITY);

        light += (directLight + ambientLight) * transmittance * density * stepSize;

        transmittance *= exp(-stepSize * ABSORPTION_SCATTERING * density);
    }
    light += transmittance * background;

    outputTexture[DTid.xy] = float4(light, 1);


}