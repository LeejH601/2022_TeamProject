#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Object.h"
#include "..\Object\Mesh.h"

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain() {};
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, CShader* pTerrainShader);
	virtual ~CHeightMapTerrain();

protected:
	CHeightMapImage* m_pHeightMapImage;

	int								m_nWidth;
	int								m_nLength;

	XMFLOAT3						m_xmf3Scale;


	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
public:
	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4 xmf4x4World);
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetRawImageWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetRawImageLength()); }


	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTerrainLayer
{
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvBaseTexture;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvAlphaBlendTexture;

	XMMATRIX					m_xmmtxTexture;
};

class CSplatTerrain : public CHeightMapTerrain
{
public:
	CSplatTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, CShader* pTerrainShader);
	virtual ~CSplatTerrain();

	virtual void SetRigidStatic();

private:
	int m_nTerrainLayers;
	std::vector<CTerrainLayer> m_vTerrainLayers;
};

class CTerrainInputStream : public physx::PxInputStream
{
	physx::PxHeightFieldDesc HeightFiledDesc;
	BYTE* m_pRawImagePixels = NULL;

public:
	CTerrainInputStream(physx::PxHeightFieldDesc& Desc, BYTE* bytes);
	virtual ~CTerrainInputStream();

	virtual uint32_t read(void* dest, uint32_t count);
};
