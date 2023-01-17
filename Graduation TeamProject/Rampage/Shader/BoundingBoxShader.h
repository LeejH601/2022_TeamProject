#pragma once
#include "Shader.h"

class CGameObject;
class CBoundingBoxShader : public CShader
{
private:
	std::vector<std::unique_ptr<CGameObject>> m_pBoundingObjects;
public:
	DECLARE_SINGLE(CBoundingBoxShader);
	void AddBoundingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState);

};