#ifndef __HLSL__
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
};
