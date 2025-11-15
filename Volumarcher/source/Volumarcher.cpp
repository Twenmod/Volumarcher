#include "Volumarcher.h"

#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"

namespace Volumarcher
{
#include "CompiledShaders/VolumetricsCS.h"


	VolumetricContext::VolumetricContext(Volume _volumes[VOLUME_AMOUNT])
	{
		m_rs.Reset(3, 0); // 1 parameter, 0 static samplers
		m_rs[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		m_rs[1].InitAsConstants(0, sizeof(VolumetricConstants) / sizeof(uint32_t));
		m_rs[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		m_rs.Finalize(L"RootSig");
		m_computePSO.SetRootSignature(m_rs);
		m_computePSO.SetComputeShader(g_pVolumetricsCS, sizeof(g_pVolumetricsCS));
		m_computePSO.Finalize();

		m_volumeBuffer.Create(L"Volume buffer", VOLUME_AMOUNT, sizeof(Volume), &_volumes[0]);
	}

	void VolumetricContext::SetVolumes(Volume _volumes[VOLUME_AMOUNT])
	{
		m_volumeBuffer.Destroy();
		m_volumeBuffer.Create(L"Volume buffer", VOLUME_AMOUNT, sizeof(Volume), &_volumes[0]);
	}

	void VolumetricContext::Render(ColorBuffer _outputBuffer, glm::vec3 _camPos, glm::quat _camRot) const
	{
		ComputeContext& computeContext = ComputeContext::Begin(L"Volumetric Pass");
		computeContext.SetPipelineState(m_computePSO);
		computeContext.SetRootSignature(m_rs);
		//Bind output
		computeContext.TransitionResource(_outputBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, false);
		computeContext.SetDynamicDescriptor(0, 0, _outputBuffer.GetUAV());
		//Bind variables

		auto screenX = _outputBuffer.GetWidth();
		auto screenY = _outputBuffer.GetHeight();

		glm::vec3 camDir = _camRot * glm::vec3(0, 0, 1);
		VolumetricConstants constants{_camPos, screenX, camDir, screenY};

		computeContext.SetConstantArray(1, sizeof(VolumetricConstants) / sizeof(uint32_t), &constants);
		computeContext.SetDynamicDescriptor(2, 0, m_volumeBuffer.GetSRV());

		computeContext.Dispatch(screenX / 32, screenY / 32, 1);
		computeContext.Finish();
	}
}
