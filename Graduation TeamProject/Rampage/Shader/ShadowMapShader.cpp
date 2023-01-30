#include "ShadowMapShader.h"
#include "..\Object\Texture.h"
#include "..\Object\Object.h"

void CShadowMapShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat, int nPipelineState)
{
	if (m_ppd3dPipelineStates.size() != 2)
	{
		m_nPipelineStates = 2;
		m_ppd3dPipelineStates.resize(m_nPipelineStates);
	}

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat, nPipelineState);
}
D3D12_INPUT_LAYOUT_DESC CShadowMapShader::CreateInputLayout(int nPipelineState)
{
	switch (nPipelineState)
	{
	case 0:
	{
		UINT nInputElementDescs = 7;
		D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

		pd3dInputElementDescs[0] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[1] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[2] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[4] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[5] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[6] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
		d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
		d3dInputLayoutDesc.NumElements = nInputElementDescs;

		return(d3dInputLayoutDesc);
	}
	case 1:
	{
		UINT nInputElementDescs = 6;
		D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

		pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[4] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		pd3dInputElementDescs[5] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
		d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
		d3dInputLayoutDesc.NumElements = nInputElementDescs;

		return(d3dInputLayoutDesc);
	}
	default:
		return D3D12_INPUT_LAYOUT_DESC();
	}
}
D3D12_SHADER_BYTECODE CShadowMapShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	switch (nPipelineState)
	{
	case 0:
		return(CShader::ReadCompiledShaderFile(L"PlayerShadowVS.cso", ppd3dShaderBlob));
	case 1:
		return(CShader::ReadCompiledShaderFile(L"TerrainShadowVS.cso", ppd3dShaderBlob));
	default:
		return D3D12_SHADER_BYTECODE();
	}
}
D3D12_SHADER_BYTECODE CShadowMapShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"ShadowMapPS.cso", ppd3dShaderBlob));
}
D3D12_DEPTH_STENCIL_DESC CShadowMapShader::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
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
void CShadowMapShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pDepthTexture) m_pDepthTexture->UpdateShaderVariables(pd3dCommandList);
}
void CShadowMapShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	m_pDepthTexture = (CTexture*)pContext;

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, m_pDepthTexture->GetTextures());
	CreateShaderResourceViews(pd3dDevice, m_pDepthTexture, 0, 9);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CShadowMapShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, 0);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pObjects)
	{
		CShader::Render(pd3dCommandList, 0);
		for (int i = 0; i < m_pObjects->size(); ++i)
		{
			(*m_pObjects)[i]->Animate(0.0f);
			(*m_pObjects)[i]->Render(pd3dCommandList, false);
		}
	}
	if (m_pTerrain)
	{
		CShader::Render(pd3dCommandList, 1);
		m_pTerrain->Render(pd3dCommandList, false);
	}
}
