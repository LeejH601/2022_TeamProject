#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Texture.h"

enum class TextureType
{
	SphereTexture,
	SmokeTexture,
	BillBoardTexture,
	ParticleTexture,
	TrailBaseTexture,
	TrailNoiseTexture,
	UITexture,
	TextureType_End
};
class CTextureManager
{
	ComPtr<ID3D12DescriptorHeap> m_pd3dCbvSrvUavDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dUavCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dUavGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dUavCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dUavGPUDescriptorNextHandle;

public:
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_pd3dSrvCpuDescriptorHandles;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dSrvGpuDescriptorHandles;	// Srv ��ũ���� �ڵ�

private:
	std::shared_ptr<CTexture> m_pTextures[(UINT)TextureType::TextureType_End];
	UINT m_iTextureN[(UINT)TextureType::TextureType_End] = { 0, };

public:
	virtual void CreateCbvSrvUavDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews, int nUnorderedAccessViews = 0);
	virtual void CreateResourceView(ID3D12Device* pd3dDevice, UINT nDescriptorHeapIndex);
	virtual void SetTextureDescriptorHeap(ID3D12GraphicsCommandList* pd3dCommandListx);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	void LoadSphereBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTexture(TextureType eTextureType, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column);
	
	int LoadTextureIndex(TextureType eTextureType, LPCTSTR pszFileName);

	std::shared_ptr<CTexture> GetTexture(TextureType eTextureType);

	UINT GetTextureListIndex(TextureType eTextureType);
	UINT GetTextureOffset(TextureType eTextureType);
public:
	CTextureManager() {
		m_pTextures[(UINT)TextureType::SphereTexture] = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::SmokeTexture] = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::BillBoardTexture] = std::make_shared<CTexture>(30, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::ParticleTexture] = std::make_shared<CTexture>(30, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::TrailBaseTexture] = std::make_shared<CTexture>(30, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::TrailNoiseTexture] = std::make_shared<CTexture>(30, RESOURCE_TEXTURE2D, 0, 1);
		m_pTextures[(UINT)TextureType::UITexture] = std::make_shared<CTexture>(30, RESOURCE_TEXTURE2D, 0, 1);
	}
	~CTextureManager() {}
};