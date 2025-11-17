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
	struct CameraSettings
	{
		//zNear of _inputDepth
		float zNear;
		//zFar of _inputDepth
		float zFar;
		//Vertical Fov in degrees
		float vFov; //TODO: Use
	};

	struct VolumetricSettings
	{
	};

	class VolumetricContext
	{
	public:
		VolumetricContext() = delete;

		explicit VolumetricContext(Volume _volumes[VOLUME_AMOUNT], CameraSettings _cameraSettings,
		                           VolumetricSettings _settings = {});

		void SetVolumes(Volume _volumes[VOLUME_AMOUNT]);

		void Render(ColorBuffer _outputBuffer, D3D12_RESOURCE_STATES _outputBufferState, DepthBuffer _inputDepth,
		            glm::vec3 _camPos = glm::vec3{0.f},
		            glm::quat _camRot = glm::identity<glm::quat>());

	private:
		//TODO: Make this user specified per volume
		CloudNoise m_noise;

		ComputePSO m_computePSO;
		RootSignature m_rs;
		StructuredBuffer m_volumeBuffer;
		CameraSettings m_cameraSettings;
		VolumetricSettings m_volumetricSettings;
	};
}
