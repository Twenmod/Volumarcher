#pragma once
#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"
#include "../shaders/ShaderCommon.h"


#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "CloudNoise.h"

namespace Volumarcher
{
	class VolumetricContext
	{
	public:
		VolumetricContext() = delete;

		explicit VolumetricContext(Volume _volumes[VOLUME_AMOUNT]);

		void SetVolumes(Volume _volumes[VOLUME_AMOUNT]);

		void Render(ColorBuffer _outputBuffer, DepthBuffer _inputDepth, glm::vec3 _camPos = glm::vec3{0.f},
		            glm::quat _camRot = glm::identity<glm::quat>());

	private:
		//TODO: Make this user specified per volume
		CloudNoise m_noise;

		ComputePSO m_computePSO;
		RootSignature m_rs;
		StructuredBuffer m_volumeBuffer;
	};
}
