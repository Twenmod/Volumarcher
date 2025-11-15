#pragma once
#include "../../MiniEngine/Core/Texture.h"

//Noise to use for a volume
class VolumeNoise
{
public:
	VolumeNoise() = default;

	[[nodiscard]] virtual Texture GetBillowNoise() = 0;

};
