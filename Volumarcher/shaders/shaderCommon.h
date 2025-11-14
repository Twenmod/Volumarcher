#ifndef __HLSL__
#include <glm/vec3.hpp>
using float3 = glm::vec3;
#else
#endif

struct VolumetricConstants
{
	float3 camPos;
	float pad0;
	float3 camDir;
};
