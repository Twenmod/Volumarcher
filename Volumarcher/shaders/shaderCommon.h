#ifndef __HLSL__
#pragma once
#include <glm/vec3.hpp>
using float3 = glm::vec3;
using uint = uint32_t;
#else
#endif

struct VolumetricConstants
{
	float3 camPos;
	uint screenResX;
	float3 camDir;
	uint screenResY;
	float zNear;
	float zFar;
};

static const uint VOLUME_AMOUNT = 1;

struct Volume
{
	float3 position;
	float squaredRad;
	float baseDensity;
	float pad0;
	float pad1;
	float pad2;
};
