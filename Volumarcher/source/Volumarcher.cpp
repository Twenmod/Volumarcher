#include "Volumarcher.h"

#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"


namespace Volumarcher
{
#include "CompiledShaders/VolumetricsCS.h"


	VolumetricContext::VolumetricContext()
	{
		m_rs.Reset(2, 0); // 1 parameter, 0 static samplers
		m_rs[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		m_rs[1].InitAsConstants(0, sizeof(glm::vec3) / sizeof(float));
		m_rs.Finalize(L"RootSig");
		m_computePSO.SetRootSignature(m_rs);
		m_computePSO.SetComputeShader(g_pVolumetricsCS, sizeof(g_pVolumetricsCS));
		m_computePSO.Finalize();
	}

	void VolumetricContext::Render(ColorBuffer _outputBuffer, glm::vec3 _camPos) const
	{
		ComputeContext& computeContext = ComputeContext::Begin(L"Volumetric Pass");
		computeContext.SetPipelineState(m_computePSO);
		computeContext.SetRootSignature(m_rs);
		//Bind output
		computeContext.TransitionResource(_outputBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, false);
		computeContext.SetDynamicDescriptor(0, 0, _outputBuffer.GetUAV());
		//Bind variables
		computeContext.SetConstants(1, _camPos.x, _camPos.y, _camPos.z);

		computeContext.Dispatch(1920 / 32, 1080 / 32, 1);
		computeContext.Finish();
	}
}
