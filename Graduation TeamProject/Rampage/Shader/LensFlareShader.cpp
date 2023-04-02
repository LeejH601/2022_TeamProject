#include "LensFlareShader.h"
#include "..\Object\Mesh.h"
#include "..\Global\Camera.h"
#include "..\Object\Texture.h"

void CLensFlareShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates.resize(m_nPipelineStates);


	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL, * pd3dGeometryShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, nPipelineState);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, nPipelineState);
	d3dPipelineStateDesc.GS = CreateGeometryShader(&pd3dGeometryShaderBlob, nPipelineState);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState(nPipelineState);
	d3dPipelineStateDesc.BlendState = CreateBlendState(nPipelineState);
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState(nPipelineState);
	d3dPipelineStateDesc.InputLayout = CreateInputLayout(nPipelineState);
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++) d3dPipelineStateDesc.RTVFormats[i] = (pdxgiRtvFormats) ? pdxgiRtvFormats[i] : DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)m_ppd3dPipelineStates[nPipelineState].GetAddressOf());

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CLensFlareShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_LENSFLARE_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbLensFlareInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedLensFlareInfo, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLensFlareInfo->Map(0, NULL, (void**)&m_pcbMappedLensFlareInfo);
}

void CLensFlareShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pTexture->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < m_nVertices; ++i) {
		m_pcbMappedLensFlareInfo->m_xmf2LensFlarePositions[i] = XMFLOAT4(m_vLensFlarePositions[i].x, m_vLensFlarePositions[i].y, 0.0f, 0.0f);
	}
	m_pcbMappedLensFlareInfo->m_xmf4UpandRight = m_xmf4UpandRightVector;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbLensFlareInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(14, d3dGpuVirtualAddress);
}

void CLensFlareShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	float offset = 0.0f;
	m_vLensFlarePositions.resize(MAX_FLARES);
	for (XMFLOAT2& pos : m_vLensFlarePositions) {
		pos.x = offset;
		pos.y = offset;
		offset += 0.05f;
	}

	m_vfLensFlareWeights.resize(MAX_FLARES);
	m_vfLensFlareWeights[0] = 0.0f;
	m_vfLensFlareWeights[1] = 0.1f;
	m_vfLensFlareWeights[2] = 0.0f;
	m_vfLensFlareWeights[3] = 0.3f;
	m_vfLensFlareWeights[4] = 0.75f;
	m_vfLensFlareWeights[5] = 0.8f;

	m_nStride = sizeof(CLensFlareVertex);
	m_nVertices = MAX_FLARES;
	std::vector<CLensFlareVertex> m_Vertices;
	m_Vertices.reserve(MAX_FLARES);
	float aspec = (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT;
	float sizeX = 0.1f;
	float sizeY = 0.2f;
	XMFLOAT2 sizes[MAX_FLARES] = {
		{1.0f,1.0f},
		{0.1f,0.1f},
		{0.7f,0.7f},
		{0.1f,0.1f},
		{0.6f,0.8f},
		{0.3f,0.3f},
	};

	for (int i = 0; i < m_nVertices; i++)
	{
		m_Vertices.push_back(CLensFlareVertex(XMFLOAT2(sizes[i].x, sizes[i].x * aspec), i));
	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dPositionBufferView.StrideInBytes = m_nStride;
	m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;

	this->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 6, 0);

	m_pTexture = std::make_unique<CTexture>(6, RESOURCE_TEXTURE2D, 0, 1);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/1.dds", RESOURCE_TEXTURE2D, 0);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/8.dds", RESOURCE_TEXTURE2D, 1);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/2_1.dds", RESOURCE_TEXTURE2D, 2);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/7.dds", RESOURCE_TEXTURE2D, 3);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/4.dds", RESOURCE_TEXTURE2D, 4);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlareImages/9.dds", RESOURCE_TEXTURE2D, 5);

	CreateShaderResourceViews(pd3dDevice, m_pTexture.get(), 0, 2);

}

void CLensFlareShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates.data() && m_ppd3dPipelineStates[nPipelineState]) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState].Get());
	if (m_pd3dCbvSrvUavDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());

	UpdateShaderVariables(pd3dCommandList);
	//CShader::UpdateShaderVariables(pd3dCommandList);
	float blendfactor[4] = { 0.0f,0.0f,0.0f,0.0f };
	pd3dCommandList->OMSetBlendFactor(blendfactor);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_pd3dPositionBufferView);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

void CLensFlareShader::CalculateFlaresPlace(CCamera* pCamera, LIGHT* pLight)
{
	XMFLOAT4 pos = XMFLOAT4(pLight->m_xmf3Position.x, pLight->m_xmf3Position.y, pLight->m_xmf3Position.z, 1.0f);
	XMVECTOR LightPos = XMLoadFloat4(&pos);
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMVECTOR lightPosInViewSpace = XMVector4Transform(LightPos, ViewMatrix);

	XMMATRIX ProjectionMatrix = XMLoadFloat4x4(&pCamera->GetProjectionMatrix());
	XMVECTOR lightPosInClipSpace = XMVector4Transform(lightPosInViewSpace, ProjectionMatrix);
	XMFLOAT4 xmf4lightPosInClipSpace; XMStoreFloat4(&xmf4lightPosInClipSpace, lightPosInClipSpace);
	XMFLOAT2 lightPosInNDC{
		xmf4lightPosInClipSpace.x / xmf4lightPosInClipSpace.w ,
		xmf4lightPosInClipSpace.y / xmf4lightPosInClipSpace.w ,
	};

	//(dot(normalize(float3(gLights[0].m_vDirection.x, 0.0f, gLights[0].m_vDirection.z)), normalize(float3(CameraDir.x, 0.0f, CameraDir.z))) > 0.0)

	XMFLOAT2 FlareVector{ (0.0f - lightPosInNDC.x) * 2, (0.0f - lightPosInNDC.y) * 2 };
	for (int i = 0; i < m_nVertices; ++i) {
		m_vLensFlarePositions[i] = XMFLOAT2(
			lightPosInNDC.x + (FlareVector.x * m_vfLensFlareWeights[i]),
			lightPosInNDC.y + (FlareVector.y * m_vfLensFlareWeights[i])
			);
	}
	float length = FlareVector.x * FlareVector.x + FlareVector.y * FlareVector.y;
	length = sqrt(length);
	XMFLOAT2 UP{ -FlareVector.x / length, -FlareVector.y / length };
	XMFLOAT2 RIGHT{ UP.y, -UP.x };
	m_xmf4UpandRightVector = XMFLOAT4(UP.x, UP.y, RIGHT.x, RIGHT.y);
}

D3D12_DEPTH_STENCIL_DESC CLensFlareShader::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CLensFlareShader::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_RASTERIZER_DESC CLensFlareShader::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE CLensFlareShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"LensFlareVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CLensFlareShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"LensFlarePixelShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CLensFlareShader::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"LensFlareGeometryShader.cso", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CLensFlareShader::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
