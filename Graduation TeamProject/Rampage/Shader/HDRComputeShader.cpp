#include "HDRComputeShader.h"

CHDRComputeShader::CHDRComputeShader()
{
}

CHDRComputeShader::~CHDRComputeShader()
{
}

D3D12_SHADER_BYTECODE CHDRComputeShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"HDRComputeShader.cso", ppd3dShaderBlob));
}

void CHDRComputeShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	/*if (!m_pSourceTexture) {
		return;
	}*/
	if (m_ppd3dPipelineStates.size() != 1)
	{
		m_nPipelineStates = 1;
		m_ppd3dPipelineStates.resize(m_nPipelineStates);
	}


	ID3DBlob* pd3dComputeShaderBlob = NULL;

	D3D12_CACHED_PIPELINE_STATE d3dCachedPipelineState = { };
	D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.CS = CreateComputeShader(&pd3dComputeShaderBlob);
	d3dPipelineStateDesc.NodeMask = 0;
	d3dPipelineStateDesc.CachedPSO = d3dCachedPipelineState;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)m_ppd3dPipelineStates[0].GetAddressOf());

	if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();


	m_cxThreadGroups = cxThreadGroups;
	m_cyThreadGroups = cyThreadGroups;
	m_czThreadGroups = czThreadGroups;

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1, 1);
	//CreateUavDescriptorHeaps(pd3dDevice, 1);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CHDRComputeShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	#ifdef _WITH_SHARED_TEXTURE
	m_pTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1, 1, 1, 1, 1, 2);
#else
	m_pTextures = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 0, 0, 1, 0, 1, 0); 
#endif
	m_pTextures->CreateTexture(pd3dDevice, FRAME_BUFFER_WIDTH,FRAME_BUFFER_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, 0);

	CreateComputeUnorderedAccessView(pd3dDevice, m_pTextures.get(), 0, 0, 0, 1);

	//m_pTextures->SetComputeSrvRootParameter(0, 0, 0, 2);
	m_pTextures->SetComputeUavRootParameter(0, 1, 0, 1);

	m_cxThreadGroups = ceil(FRAME_BUFFER_WIDTH / 32.0f);
	m_cyThreadGroups = ceil(FRAME_BUFFER_HEIGHT / 32.0f);
}

void CHDRComputeShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pTextures) {
		m_pSourceTexture->UpdateComputeSrvShaderVariable(pd3dCommandList, 0);
		m_pTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
	}
}

void CHDRComputeShader::ReleaseShaderVariables()
{
}

void CHDRComputeShader::ReleaseUploadBuffers()
{
}

void CHDRComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList, 0);
	//UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->Dispatch(m_cxThreadGroups, m_cyThreadGroups, m_czThreadGroups);
}
