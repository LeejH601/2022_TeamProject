#pragma once
#include "ComputeShader.h"
#include "..\Object\Texture.h"

class CBloomShader : public CComputeShader
{
public:
	CBloomShader();
	virtual ~CBloomShader();

	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups = 1, UINT cyThreadGroups = 1, UINT czThreadGroups = 1);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateBloomUAVResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT xResoution, UINT yResoultion, int nDownSampling = 0);

public:
	std::shared_ptr<CTexture> m_pSourceTextures;
	std::unique_ptr<CTexture> m_pFillterTextures;
	std::unique_ptr<CTexture> m_pBloomedTexture;
};

