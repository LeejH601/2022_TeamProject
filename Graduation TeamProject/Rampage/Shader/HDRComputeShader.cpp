#include "HDRComputeShader.h"

CHDRComputeShader::CHDRComputeShader()
{
}

CHDRComputeShader::~CHDRComputeShader()
{
}

D3D12_SHADER_BYTECODE CHDRComputeShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
    return D3D12_SHADER_BYTECODE();
}

void CHDRComputeShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	if (m_ppd3dPipelineStates.size() != 1)
	{
		m_nPipelineStates = 1;
		m_ppd3dPipelineStates.resize(m_nPipelineStates);
	}

	CComputeShader::CreateShader(pd3dDevice, pd3dRootSignature, cxThreadGroups, cyThreadGroups, czThreadGroups);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	CreateUavDescriptorHeaps(pd3dDevice, 1);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CHDRComputeShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
//	#ifdef _WITH_SHARED_TEXTURE
//	m_pTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1, 1, 1, 1, 1, 2);
//#else
//	m_pTextures = std::make_unique<CTexture>(2, RESOURCE_TEXTURE2D,)
//	m_pTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 0, 1, 1, 0, 1, 2);
//#endif
//	m_pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/InputA.dds", RESOURCE_TEXTURE2D, 0); //1024x1024
//	m_pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/InputC.dds", RESOURCE_TEXTURE2D, 1); //1024x1024
//	ID3D12Resource *pd3dResource = m_pTexture->GetResource(0);
//	D3D12_RESOURCE_DESC d3dResourceDesc = pd3dResource->GetDesc();
//	m_pTexture->CreateTexture(pd3dDevice, d3dResourceDesc.Width, d3dResourceDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, 2);
//
//	CreateComputeShaderResourceView(pd3dDevice, m_pTexture, 0, 0, 0, 2);
//	CreateComputeUnorderedAccessView(pd3dDevice, m_pTexture, 2, 0, 0, 1);
//
//	m_pTexture->SetComputeSrvRootParameter(0, 0, 0, 2);
//	m_pTexture->SetComputeUavRootParameter(0, 1, 0, 1);
//
//	m_cxThreadGroups = ceil(d3dResourceDesc.Width / 32.0f);
//	m_cyThreadGroups = ceil(d3dResourceDesc.Height / 32.0f); 
}

void CHDRComputeShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CHDRComputeShader::ReleaseShaderVariables()
{
}

void CHDRComputeShader::ReleaseUploadBuffers()
{
}

void CHDRComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
