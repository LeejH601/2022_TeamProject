#include "SwordTrailShader.h"
#include "..\Object\Texture.h"

CSwordTrailShader::CSwordTrailShader()
{
	ZeroMemory(&m_xmf4x4SwordTrailControllPointers, sizeof(XMFLOAT4X4));
	m_xmf4TrailControllPoints1.resize(MAX_TRAILCONTROLLPOINTS);
	m_xmf4TrailControllPoints2.resize(MAX_TRAILCONTROLLPOINTS);

	m_xmf4TrailBasePoints1.resize(14);
	m_xmf4TrailBasePoints2.resize(14);
}

void CSwordTrailShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState)
{
	if (nPipelineState == 0) {
		CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, nPipelineState);
	}
	else {
		ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL, * pd3dGeometryShaderBlob = NULL;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
		::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, nPipelineState);
		d3dPipelineStateDesc.GS = CreateGeometryShader(&pd3dGeometryShaderBlob, nPipelineState);
		d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, nPipelineState);
		d3dPipelineStateDesc.StreamOutput = CreateStreamOuputState(nPipelineState);
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
}

D3D12_BLEND_DESC CSwordTrailShader::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_RASTERIZER_DESC CSwordTrailShader::CreateRasterizerState(int nPipelineState)
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

D3D12_DEPTH_STENCIL_DESC CSwordTrailShader::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

D3D12_SHADER_BYTECODE CSwordTrailShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	if (nPipelineState == 0)
		return(CShader::ReadCompiledShaderFile(L"PostProcessingVertexShader.cso", ppd3dShaderBlob));
	else
		return(CShader::ReadCompiledShaderFile(L"SwordTrailVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CSwordTrailShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	if (nPipelineState == 0)
		return(CShader::ReadCompiledShaderFile(L"ClearTargetPixelShader.cso", ppd3dShaderBlob));
	else
		return(CShader::ReadCompiledShaderFile(L"SwordTrailPixelShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CSwordTrailShader::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	if (nPipelineState == 0)
		return(CShader::CreateGeometryShader(ppd3dShaderBlob, nPipelineState));
	else
		return(CShader::ReadCompiledShaderFile(L"SwordTrailGeometryShader.cso", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CSwordTrailShader::CreateInputLayout(int nPipelineState)
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	if (nPipelineState == 0)
	{
		d3dInputLayoutDesc.pInputElementDescs = NULL;
		d3dInputLayoutDesc.NumElements = 0;
	}
	else {
		d3dInputLayoutDesc.pInputElementDescs = NULL;
		d3dInputLayoutDesc.NumElements = 0;

		/*UINT nInputElementDescs = 4;
		D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

		pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[1] = { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[2] = { "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[3] = { "ALPHA", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
		d3dInputLayoutDesc.NumElements = nInputElementDescs;*/
	}

	return(d3dInputLayoutDesc);
}

void CSwordTrailShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 6, 0);

	m_pTexture = std::make_unique<CTexture>(2, RESOURCE_TEXTURE2D, 0, 1);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/T_Sword_Slash_21.dds", RESOURCE_TEXTURE2D, 0);
	m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/VAP1_Noise_14.dds", RESOURCE_TEXTURE2D, 1);

	CreateShaderResourceViews(pd3dDevice, m_pTexture.get(), 0, 2);
}

void CSwordTrailShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_SWTRAIL_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbTrail = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedTrail, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbTrail->Map(0, NULL, (void**)&m_pcbMappedTrail);
}

void CSwordTrailShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pTexture)
		m_pTexture->UpdateShaderVariables(pd3dCommandList);

	memcpy(m_pcbMappedTrail->m_xmf4TrailControllPoints1, m_xmf4TrailControllPoints1.data(), sizeof(m_pcbMappedTrail->m_xmf4TrailControllPoints1));
	memcpy(m_pcbMappedTrail->m_xmf4TrailControllPoints2, m_xmf4TrailControllPoints2.data(), sizeof(m_pcbMappedTrail->m_xmf4TrailControllPoints2));
	m_pcbMappedTrail->m_nDrawedControllPoints = m_nDrawedControllPoints;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbTrail->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);
}

void CSwordTrailShader::ReleaseShaderVariables()
{
	if (m_pd3dcbTrail)
		m_pd3dcbTrail->Unmap(0, NULL);
}

void CSwordTrailShader::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates.resize(m_nPipelineStates);

	DXGI_FORMAT pdxgiRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	CSwordTrailShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 7, pdxgiRtvFormats, 0);
	CSwordTrailShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 7, pdxgiRtvFormats, 1);
}

void CSwordTrailShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	//CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	//if (nPipelineState == 0) {
	//	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	//}
	//else {

	//	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4SwordTrailControllPointers, 0);

	//	memcpy(&m_xmf4x4SwordTrailControllPointers._11, &m_xmf4x4SwordTrailControllPointers._31, sizeof(XMFLOAT4));
	//	memcpy(&m_xmf4x4SwordTrailControllPointers._21, &m_xmf4x4SwordTrailControllPointers._41, sizeof(XMFLOAT4));

	//	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
	//}
	if (m_nDrawedControllPoints < 2)
		return;

	CShader::Render(pd3dCommandList, pCamera, 1);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(m_nDrawedControllPoints - 1, 1, 0, 0);
}

void CSwordTrailShader::SetNextControllPoint(XMFLOAT4 point1, XMFLOAT4 point2)
{
	//memcpy(&m_xmf4x4SwordTrailControllPointers, &m_xmf4x4SwordTrailControllPointers + (sizeof(XMFLOAT4X4) / 2), (sizeof(XMFLOAT4X4) / 2));

	//memcpy(&m_xmf4x4SwordTrailControllPointers._11, &m_xmf4x4SwordTrailControllPointers._31, sizeof(XMFLOAT4));
	/*m_xmf4x4SwordTrailControllPointers._11 = m_xmf4x4SwordTrailControllPointers._31;
	m_xmf4x4SwordTrailControllPointers._12 = m_xmf4x4SwordTrailControllPointers._32;
	m_xmf4x4SwordTrailControllPointers._13 = m_xmf4x4SwordTrailControllPointers._33;
	m_xmf4x4SwordTrailControllPointers._14 = m_xmf4x4SwordTrailControllPointers._34;*/

	//memcpy(&m_xmf4x4SwordTrailControllPointers._21, &m_xmf4x4SwordTrailControllPointers._41, sizeof(XMFLOAT4));
	/*m_xmf4x4SwordTrailControllPointers._21 = m_xmf4x4SwordTrailControllPointers._41;
	m_xmf4x4SwordTrailControllPointers._22 = m_xmf4x4SwordTrailControllPointers._42;
	m_xmf4x4SwordTrailControllPointers._23 = m_xmf4x4SwordTrailControllPointers._43;
	m_xmf4x4SwordTrailControllPointers._24 = m_xmf4x4SwordTrailControllPointers._44;*/

	memcpy(&m_xmf4x4SwordTrailControllPointers._31, &point2, sizeof(XMFLOAT4));
	/*m_xmf4x4SwordTrailControllPointers._31 = point1.x;
	m_xmf4x4SwordTrailControllPointers._32 = point1.y;
	m_xmf4x4SwordTrailControllPointers._33 = point1.z;
	m_xmf4x4SwordTrailControllPointers._34 = point1.w;*/

	memcpy(&m_xmf4x4SwordTrailControllPointers._41, &point1, sizeof(XMFLOAT4));
	/*m_xmf4x4SwordTrailControllPointers._41 = point2.x;
	m_xmf4x4SwordTrailControllPointers._42 = point2.y;
	m_xmf4x4SwordTrailControllPointers._43 = point2.z;
	m_xmf4x4SwordTrailControllPointers._44 = point2.w;*/

	// 기본
	/*if (m_nDrawedControllPoints < MAX_TRAILCONTROLLPOINTS) {
		m_xmf4TrailControllPoints1[m_nDrawedControllPoints] = point1;
		m_xmf4TrailControllPoints2[m_nDrawedControllPoints++] = point2;
	}
	else {
		memcpy(m_xmf4TrailControllPoints1.data(), m_xmf4TrailControllPoints1.data() + 1, (m_nDrawedControllPoints - 1) * sizeof(XMFLOAT4));
		memcpy(m_xmf4TrailControllPoints2.data(), m_xmf4TrailControllPoints2.data() + 1, (m_nDrawedControllPoints - 1) * sizeof(XMFLOAT4));

		m_xmf4TrailControllPoints1[m_nDrawedControllPoints - 1] = point1;
		m_xmf4TrailControllPoints2[m_nDrawedControllPoints - 1] = point2;
	}*/

	// 스플라인
	int dummy = 1;
	if (m_nTrailBasePoints < 10) {
		m_xmf4TrailBasePoints1[dummy + m_nTrailBasePoints] = point1;
		m_xmf4TrailBasePoints2[dummy + m_nTrailBasePoints++] = point2;
	}
	else {
		memcpy(m_xmf4TrailBasePoints1.data() + dummy, m_xmf4TrailBasePoints1.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));
		memcpy(m_xmf4TrailBasePoints2.data() + dummy, m_xmf4TrailBasePoints2.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));

		m_xmf4TrailBasePoints1[dummy + m_nTrailBasePoints - 1] = point1;
		m_xmf4TrailBasePoints2[dummy + m_nTrailBasePoints - 1] = point2;
	}

	m_xmf4TrailBasePoints1[0] = m_xmf4TrailBasePoints1[1];
	m_xmf4TrailBasePoints2[0] = m_xmf4TrailBasePoints2[1];

	for (int i = 1; i <= 3; ++i) {
		m_xmf4TrailBasePoints1[m_nTrailBasePoints + i] = m_xmf4TrailBasePoints1[m_nTrailBasePoints];
		m_xmf4TrailBasePoints2[m_nTrailBasePoints + i] = m_xmf4TrailBasePoints2[m_nTrailBasePoints];
	}

	int index = 0;
	for (int i = 0; i < 10; ++i) {
		m_xmf4TrailControllPoints1[index] = m_xmf4TrailBasePoints1[i + 1];
		m_xmf4TrailControllPoints2[index++] = m_xmf4TrailBasePoints2[i + 1];
		for (int j = 1; j < 10; ++j) {
			float t = j * 0.10f;
			XMVECTOR points[4];
			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 3]);

			XMStoreFloat4(&m_xmf4TrailControllPoints1[index], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));

			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 3]);

			XMStoreFloat4(&m_xmf4TrailControllPoints2[index++], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));
		}
	}

	m_nDrawedControllPoints = index;

	printf("dsg");
}