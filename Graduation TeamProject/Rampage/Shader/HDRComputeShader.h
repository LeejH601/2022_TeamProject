#pragma once
#include "ComputeShader.h"
#include "..\Object\Texture.h"

class CHDRComputeShader : public CComputeShader
{
public:
	CHDRComputeShader();
	virtual ~CHDRComputeShader();

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups = 1, UINT cyThreadGroups = 1, UINT czThreadGroups = 1);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	void SetTextureSource(ID3D12Device* pd3dDevice, std::shared_ptr<CTexture> pSourceTexture) {
		m_pSourceTexture = pSourceTexture; 
		CreateComputeShaderResourceView(pd3dDevice, m_pSourceTexture.get(), 0, 0, 0, 1);
		m_pSourceTexture->SetComputeSrvRootParameter(0, 0, 0, 1);
	};

	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	std::shared_ptr<CTexture> m_pSourceTexture;
	std::unique_ptr<CTexture> m_pTextures;
};

