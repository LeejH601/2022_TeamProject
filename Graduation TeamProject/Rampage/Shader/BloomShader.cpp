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

D3D12_SHADER_BYTECODE CBloomShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	if(nPipelineState == 0)
		return(CShader::ReadCompiledShaderFile(L"BloomComputeShader.cso", ppd3dShaderBlob));
	else if(nPipelineState == 1)
		return(CShader::ReadCompiledShaderFile(L"BloomApplyComputeShader.cso", ppd3dShaderBlob));
	else if(nPipelineState == 2)
		return(CShader::ReadCompiledShaderFile(L"BloomAdditiveComputeShader.cso", ppd3dShaderBlob));
}

void CBloomShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	if (m_ppd3dPipelineStates.size() != 3)
	{
		m_nPipelineStates = 2;
		m_ppd3dPipelineStates.resize(m_nPipelineStates);
	}


	ID3DBlob* pd3dComputeShaderBlob = NULL;

	D3D12_CACHED_PIPELINE_STATE d3dCachedPipelineState = { };
	D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.CS = CreateComputeShader(&pd3dComputeShaderBlob, 0);
	d3dPipelineStateDesc.NodeMask = 0;
	d3dPipelineStateDesc.CachedPSO = d3dCachedPipelineState;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)m_ppd3dPipelineStates[0].GetAddressOf());

	if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();

	D3D12_CACHED_PIPELINE_STATE d3dCachedPipelineState2 = { };
	ID3DBlob* pd3dComputeShaderBlob2 = NULL;
	D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc2;
	::ZeroMemory(&d3dPipelineStateDesc2, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc2.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc2.CS = CreateComputeShader(&pd3dComputeShaderBlob2, 1);
	d3dPipelineStateDesc2.NodeMask = 0;
	d3dPipelineStateDesc2.CachedPSO = d3dCachedPipelineState2;
	d3dPipelineStateDesc2.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	//d3dPipelineStateDesc2.CS = CreateComputeShader(&pd3dComputeShaderBlob, 1);
	hResult = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc2, __uuidof(ID3D12PipelineState), (void**)m_ppd3dPipelineStates[1].GetAddressOf());

	if (pd3dComputeShaderBlob2) pd3dComputeShaderBlob2->Release();

	d3dPipelineStateDesc2.CS = CreateComputeShader(&pd3dComputeShaderBlob2, 2);
	hResult = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc2, __uuidof(ID3D12PipelineState), (void**)m_ppd3dPipelineStates[2].GetAddressOf());

	if (pd3dComputeShaderBlob2) pd3dComputeShaderBlob2->Release();

	m_cxThreadGroups = cxThreadGroups;
	m_cyThreadGroups = cyThreadGroups;
	m_czThreadGroups = czThreadGroups;

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1 + 5, 1 + 5 + 5);
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

	if (m_pSourceTextures) {
		m_pSourceTextures->UpdateComputeSrvShaderVariable(pd3dCommandList, 0);
	}
	if (m_pFillterTextures) {
		m_pFillterTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
	}
	if (m_pBloomedTexture) {
		m_pBloomedTexture->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
	}
	if (m_pBluredTextures) {
		m_pBluredTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
	}
}

void CBloomShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	if (nPipelineState == 0) {
		if (m_pSourceTextures) {
			m_pSourceTextures->UpdateComputeSrvShaderVariable(pd3dCommandList, 0);
		}
		if (m_pFillterTextures) {
			m_pFillterTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBloomedTexture) {
			m_pBloomedTexture->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBluredTextures) {
			m_pBluredTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
	}
	else if (nPipelineState == 1) {
		if (m_pSourceTextures) {
			m_pSourceTextures->UpdateComputeSrvShaderVariable(pd3dCommandList, 0);
		}
		if (m_pFillterTextures) {
			m_pFillterTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBluredTextures) {
			m_pBluredTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBloomedTexture) {
			m_pBloomedTexture->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
	}
	else if (nPipelineState == 2) {
		if (m_pSourceTextures) {
			m_pSourceTextures->UpdateComputeSrvShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBluredTextures) {
			m_pBluredTextures->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
		if (m_pBloomedTexture) {
			m_pBloomedTexture->UpdateComputeUavShaderVariable(pd3dCommandList, 0);
		}
	}
}

void CBloomShader::ReleaseShaderVariables()
{
}

void CBloomShader::ReleaseUploadBuffers()
{
}

void CBloomShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int redutionSize = 4;

	if (m_ppd3dPipelineStates.data() && m_ppd3dPipelineStates[0]) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0].Get());
	if (m_pd3dCbvSrvUavDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());
	UpdateShaderVariables(pd3dCommandList, 0);

	pd3dCommandList->Dispatch(m_cxThreadGroups, m_cyThreadGroups, m_czThreadGroups);



	if (m_ppd3dPipelineStates.data() && m_ppd3dPipelineStates[1]) 
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[1].Get());
	if (m_pd3dCbvSrvUavDescriptorHeap) 
		pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());
	UpdateShaderVariables(pd3dCommandList, 1);

	for (int i = 3; i >= 1; --i) {
		XMFLOAT4 level = { (float)i,0,0,0 };

		/*if (m_pd3dCbvSrvUavDescriptorHeap)
			pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());*/
		pd3dCommandList->SetComputeRoot32BitConstants(5, 4, &level, 0);

		//UpdateShaderVariables(pd3dCommandList, 1);
		pd3dCommandList->Dispatch(1920 / pow(redutionSize, i), 1080 / pow(redutionSize,i), 1);
	}

	if (m_ppd3dPipelineStates.data() && m_ppd3dPipelineStates[2])
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[2].Get());
	if (m_pd3dCbvSrvUavDescriptorHeap)
		pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());
	UpdateShaderVariables(pd3dCommandList, 2);

	pd3dCommandList->Dispatch(m_cxThreadGroups, m_cyThreadGroups, m_czThreadGroups);
}

void CBloomShader::CreateBloomUAVResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT xResoution, UINT yResoultion, int nDownSampling)
{
	int redutionSize = 4;

	UINT minResoul = xResoution > yResoultion ? yResoultion : xResoution;
	int maxDownSample = 4;
	int nDownSample = maxDownSample;
	for (int i = 0; i < maxDownSample; ++i) {
		if (minResoul / (pow(redutionSize, maxDownSample - i)) < 16) {
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
		resoultion.x /= redutionSize;
		resoultion.y /= redutionSize;
		resoultion.x = ceil(resoultion.x);
		resoultion.y = ceil(resoultion.y);
		downSampleTextureResoultions.emplace_back(resoultion);
	}

	m_nFillters = nDownSample + 1;
	m_pFillterTextures = std::make_unique<CTexture>(nDownSample + 1, RESOURCE_TEXTURE2D, 0, 0, 0, 1, 0, nDownSample + 1, 0);
	m_pBluredTextures = std::make_unique<CTexture>(nDownSample + 1, RESOURCE_TEXTURE2D, 0, 0, 0, 1, 1, nDownSample + 1, nDownSample + 1);
	for (int i = 0; i < nDownSample + 1; ++i) {
		m_pFillterTextures->CreateTexture(pd3dDevice, downSampleTextureResoultions[i].x, downSampleTextureResoultions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, i);
		m_pBluredTextures->CreateTexture(pd3dDevice, downSampleTextureResoultions[i].x, downSampleTextureResoultions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, NULL, RESOURCE_TEXTURE2D, i);
	}
	CreateComputeUnorderedAccessView(pd3dDevice, m_pFillterTextures.get(), 0, 0, 0, nDownSample + 1);
	m_pFillterTextures->SetComputeUavRootParameter(0, 2, 0, nDownSample + 1);

	//CreateComputeShaderResourceView(pd3dDevice, m_pBluredTextures.get(), 0, 0, 0, nDownSample + 1);
	CreateComputeUnorderedAccessView(pd3dDevice, m_pBluredTextures.get(), 0, 0, 0, nDownSample + 1);
	m_pBluredTextures->SetComputeUavRootParameter(0, 3, 0, nDownSample + 1);
	//m_pBluredTextures->SetComputeSrvRootParameter(0, 4, 0, nDownSample + 1);
}

void CBloomShader::ClearFillters(ID3D12GraphicsCommandList* pd3dCommandList)
{
	
}
