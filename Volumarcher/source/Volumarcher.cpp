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
		m_rs.Reset(1, 0); // 1 parameter, 0 static samplers
		m_rs[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		m_rs.Finalize(L"RootSig");
		m_computePSO.SetRootSignature(m_rs);
		m_computePSO.SetComputeShader(g_pVolumetricsCS, sizeof(g_pVolumetricsCS));
		m_computePSO.Finalize();
	}

	void VolumetricContext::Render(ColorBuffer _outputBuffer) const
	{
		ComputeContext& computeContext = ComputeContext::Begin(L"Volumetric Pass");
		computeContext.SetPipelineState(m_computePSO);
		computeContext.SetRootSignature(m_rs);
		computeContext.TransitionResource(_outputBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		computeContext.SetDynamicDescriptor(0, 0, _outputBuffer.GetUAV());
		computeContext.Dispatch(1920, 1080, 1);
		computeContext.Finish();
	}
}
