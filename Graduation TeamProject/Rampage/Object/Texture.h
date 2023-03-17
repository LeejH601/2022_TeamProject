#pragma once
#include "..\Global\stdafx.h"

class CShader;
class CGameObject;
class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	// 개수, 텍스쳐 포맷, 그래픽 루트파라미터 개수, 그래픽 SRV 핸들 개수, 컴퓨트 UAV 파라미터 개수, 컴퓨트 SRV 파라미터 개수, 컴퓨트 UAV 핸들 개수, 컴퓨트 SRV 핸들 개수
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters, int nGraphicsSrvGpuHandles, int nComputeUavRootParameters, int nComputeSrvRootParameters, int nComputeUavGpuHandles, int nComputeSrvGpuHandles);
	virtual ~CTexture();
	UINT m_nTextureType;		// Texture 타입

private:
	int	m_nTextures = 0;		// Texture 개수

	int m_iRow = 1;				// 스프라이트 열 개수
	int m_iColumn = 1;			// 스프라이트 행 개수

	std::vector<ComPtr<ID3D12Resource>> m_ppd3dTextures;	// Texture 리소스
	std::vector<ComPtr<ID3D12Resource>> m_ppd3dTextureUploadBuffers;	//Texture 업로드 버퍼

	std::vector<UINT> m_pnResourceTypes;		// 각각의 Texture 들의 타입
	std::vector<std::wstring> m_ppstrTextureNames;	// Texture의 이름

	std::vector<DXGI_FORMAT> m_pdxgiBufferFormats;	// Texture Dxgi 포맷
	std::vector<int> m_pnBufferElements;
	std::vector<int> m_pnBufferStrides;

	int	m_nRootParameters = 0;		// 루트파라미터 번호
	std::vector<int> m_pnRootParameterIndices;		// 루트파라미터 인덱스

	int	m_nComputeUavRootParameters = 0;
	std::vector<int> m_pnComputeUavRootParameterIndices;
	std::vector<int> m_pnComputeUavRootParameteDescriptors;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dComputeUavRootParameterGpuDescriptorHandles;

	int	m_nComputeSrvRootParameters = 0;
	std::vector<int> m_pnComputeSrvRootParameterIndices;
	std::vector<int> m_pnComputeSrvRootParameterDescriptors;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dComputeSrvRootParameterGpuDescriptorHandles;

public:
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dSrvGpuDescriptorHandles;	// Srv 디스크립터 핸들
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dComputeUavGpuDescriptorHandles; // compute Uav 디스크립터 핸들
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_pd3dComputeSrvGpuDescriptorHandles;  // compute Srv 디스크립터 핸들


public:

	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth,
		UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags,
		D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { 
		return(m_ppd3dTextures[nIndex].Get()); 
	}
	const wchar_t* GetTextureName(int nIndex) 
	{ 
		return(m_ppstrTextureNames[nIndex].data());
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { if (nIndex < m_pnRootParameterIndices.size()) return(m_pnRootParameterIndices[nIndex]); return -1; }
	int GetRootParameters() { return(m_nRootParameters); }
	UINT GetTextureType() { return(m_nTextureType); }

	int GetComputeSrvRootParameters() { return(m_nComputeSrvRootParameters); }
	void SetComputeSrvRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nSrvDescriptors);
	int GetComputeSrvRootParameterIndex(int nIndex) { return(m_pnComputeSrvRootParameterIndices[nIndex]); }

	int GetComputeUavRootParameters() { return(m_nComputeUavRootParameters); }
	void SetComputeUavRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nUavDescriptors);
	int GetComputeUavRootParameterIndex(int nIndex) { return(m_pnComputeUavRootParameterIndices[nIndex]); }

	void SetComputeSrvGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetComputeSrvGpuDescriptorHandle(int nHandleIndex) { return(m_pd3dComputeSrvGpuDescriptorHandles[nHandleIndex]); }

	void SetComputeUavGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetComputeUavGpuDescriptorHandle(int nHandleIndex) { return(m_pd3dComputeUavGpuDescriptorHandles[nHandleIndex]); }

	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateComputeSrvShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);
	void UpdateComputeUavShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);

	void ReleaseShaderVariables();

	void CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT dxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);
	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	int LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex);
	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(int nIndex);

	void ReleaseUploadBuffers();

	void AnimateRowColumn(float fTime = 0.0f);
	void SetRowColumn(int iRow, int iColumn);
	int GetRow();
	int GetColumn();
};
class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
public:
	std::shared_ptr<CTexture> m_pTexture = NULL;

	XMFLOAT4 m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(std::shared_ptr<CShader> pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(std::shared_ptr<CTexture> pTexture);
	std::shared_ptr<CTexture> GetTexture() { return m_pTexture; }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();
public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	std::shared_ptr<CShader> m_pShader;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////