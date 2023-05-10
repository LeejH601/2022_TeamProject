#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Texture.h"

enum class TextureType
{
	SphereTexture,
	BillBoardTexture,
	ParticleTexture
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
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dSrvGpuDescriptorHandles;	// Srv 디스크립터 핸들

private:
	std::shared_ptr<CTexture> m_pTextures;
	int m_iBillBoardTextureN = 1;
	int m_iParticleTextureN = 0;
	int m_iSphereTextureN = 0;
	

	std::pair<int, int> m_iBillBoardTexture;
	std::pair<int, int> m_iParticleTexture;
	std::pair<int, int> m_iSphereTexture;

public:
	virtual void CreateCbvSrvUavDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews, int nUnorderedAccessViews = 0);
	virtual void CreateResourceView(ID3D12Device* pd3dDevice, UINT nDescriptorHeapIndex);
	virtual void SetTextureDescriptorHeap(ID3D12GraphicsCommandList* pd3dCommandListx);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	void LoadSphereBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column);
	void LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column);
	
	int LoadTextureIndex(LPCTSTR pszFileName);

	std::shared_ptr<CTexture> GetTexture();

	std::pair<int, int> GetTextureListIndex(TextureType eTextureType);
public:
	CTextureManager() {
		m_pTextures = std::make_shared<CTexture>(100, RESOURCE_TEXTURE2D, 0, 1);

		m_iBillBoardTexture = std::make_pair(0, 0);
		m_iParticleTexture = std::make_pair(0, 0);
		m_iSphereTexture = std::make_pair(0, 0);
	}
	~CTextureManager() {}

	//// BillBoardTextures
	//std::shared_ptr<CTexture> LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, class CBillBoardObjectShader* pShader, int iRow, int Column);
	//std::shared_ptr<CTexture> LoadBillBoardTexture(LPCTSTR pszFileName);

	//std::vector<std::shared_ptr<CTexture>>& GetBillBoardTextureList();

	//// ParticleTextures
	//std::shared_ptr<CTexture> LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, class CParticleShader* pShader, int iRow, int Column);
	//std::shared_ptr<CTexture> LoadParticleTexture(LPCTSTR pszFileName);

	//std::vector<std::shared_ptr<CTexture>>& GetParticleTextureList();
//protected:
//	std::vector<std::shared_ptr<CTexture>> m_vBillBoardTextures;
//	std::vector<std::shared_ptr<CTexture>> m_vParticleTextures;
};