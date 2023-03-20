#include "BloomShader.h"

CBloomShader::CBloomShader()
{
}

CBloomShader::~CBloomShader()
{
}

D3D12_SHADER_BYTECODE CBloomShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"BloomComputeShader.cso", ppd3dShaderBlob));
}

void CBloomShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
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

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1, 1 + 5);
	//CreateUavDescriptorHeaps(pd3dDevice, 1);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CBloomShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
#ifdef _WITH_SHARED_TEXTURE
	m_pTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1, 1, 1, 1, 1, 2);
#else
	m_pBloomedTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 0, 0, 1, 0, 1, 0);
#endif
	m_pBloomedTexture->CreateTexture(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, 0);

	CreateComputeUnorderedAccessView(pd3dDevice, m_pBloomedTexture.get(), 0, 0, 0, 1);

	//m_pTextures->SetComputeSrvRootParameter(0, 0, 0, 2);
	m_pBloomedTexture->SetComputeUavRootParameter(0, 1, 0, 1);

	m_cxThreadGroups = ceil(FRAME_BUFFER_WIDTH / 32.0f);
	m_cyThreadGroups = ceil(FRAME_BUFFER_HEIGHT / 32.0f);
}

void CBloomShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CBloomShader::ReleaseShaderVariables()
{
}

void CBloomShader::ReleaseUploadBuffers()
{
}

void CBloomShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CBloomShader::CreateBloomUAVResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT xResoution, UINT yResoultion, int nDownSampling)
{
	UINT minResoul = xResoution > yResoultion ? yResoultion : xResoution;
	int maxDownSample = 4;
	int nDownSample = maxDownSample;
	for (int i = 0; i < maxDownSample; ++i) {
		if (minResoul / (pow(4, maxDownSample - i)) < 16) {
			nDownSample--;
		}
		else
			break;
	}

	std::vector<XMFLOAT2> downSampleTextureResoultions;
	XMFLOAT2 baseResoultion{ float(xResoution),  float(yResoultion) };

	downSampleTextureResoultions.emplace_back(xResoution, yResoultion);
	XMFLOAT2 resoultion = baseResoultion;
	for (int i = 0; i < nDownSample; ++i) {
		resoultion.x /= 4;
		resoultion.y /= 4;
		resoultion.x = ceil(resoultion.x);
		resoultion.y = ceil(resoultion.y);
		downSampleTextureResoultions.emplace_back(resoultion);
	}

	m_pFillterTextures = std::make_unique<CTexture>(nDownSample + 1, RESOURCE_TEXTURE2D, 0, 0, 0, 1, 0, nDownSample + 1, 0);
	for (int i = 0; i < nDownSample + 1; ++i) {
		m_pFillterTextures->CreateTexture(pd3dDevice, downSampleTextureResoultions[i].x, downSampleTextureResoultions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, i);
	}
	CreateComputeUnorderedAccessView(pd3dDevice, m_pFillterTextures.get(), 0, 0, 0, nDownSample + 1);
	m_pFillterTextures->SetComputeUavRootParameter(0, 2, 1, nDownSample + 1);
}
