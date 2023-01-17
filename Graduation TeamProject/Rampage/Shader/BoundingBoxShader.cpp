#include "BoundingBoxShader.h"
#include "..\Object\Object.h"

void CBoundingBoxShader::AddBoundingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents)
{
	std::unique_ptr<CBoundingBoxObject> pObject = std::make_unique<CBoundingBoxObject>(pd3dDevice, pd3dCommandList, xmf3AABBCenter, xmf3AABBExtents);
	pObject->SetParent(pParent);
	m_pBoundingObjects.push_back(std::move(pObject));
}
void CBoundingBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates.resize(m_nPipelineStates);

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, 0);
}
D3D12_SHADER_BYTECODE CBoundingBoxShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"BoundVertexShader.cso", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CBoundingBoxShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"BoundPixelShader.cso", ppd3dShaderBlob));
}
D3D12_INPUT_LAYOUT_DESC CBoundingBoxShader::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
void CBoundingBoxShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	CShader::Render(pd3dCommandList, nPipelineState);
	for (int i = 0; i < m_pBoundingObjects.size(); ++i)
	{
		m_pBoundingObjects[i]->Render(pd3dCommandList);
	}
}
