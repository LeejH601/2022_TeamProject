#pragma once
#include "..\Global\stdafx.h"

#define VERTEXT_POSITION				0x01
#define VERTEXT_COLOR					0x02
#define VERTEXT_NORMAL					0x04
#define VERTEXT_TANGENT					0x08
#define VERTEXT_TEXTURE_COORD0			0x10
#define VERTEXT_TEXTURE_COORD1			0x20

class CMesh
{
protected:
	UINT m_nType = 0x00;
	UINT m_nVertices = 0;

	std::vector<XMFLOAT3> m_pxmf3Positions;
	ComPtr<ID3D12Resource> m_pd3dPositionBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dPositionUploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3Normals;
	ComPtr<ID3D12Resource> m_pd3dNormalBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dNormalUploadBuffer = NULL;

	UINT m_nVertexBufferViews = 0;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> m_pd3dVertexBufferViews;

	UINT m_nIndices = 0;
	std::vector<UINT> m_pnIndices;

	std::vector<UINT> m_pnSubSetIndices;
	std::vector<UINT> m_pnSubSetStartIndices;
	std::vector<std::vector<UINT>> m_ppnSubSetIndices;

	std::vector<ComPtr<ID3D12Resource>> m_ppd3dIndexBuffers;
	std::vector<ComPtr<ID3D12Resource>> m_ppd3dIndexUploadBuffers;
	std::vector<D3D12_INDEX_BUFFER_VIEW> m_pd3dIndexBufferViews;

	D3D12_PRIMITIVE_TOPOLOGY        m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT                            m_nSlot = 0;
	UINT                            m_nSubMeshes = 0;
	UINT                            m_nStride = 0;
	UINT                            m_nOffset = 0;
	UINT                            m_nStartIndex = 0;
	int                             m_nBaseVertex = 0;
	BoundingBox                     m_xmBoundingBox;
public:
	CMesh() {};
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName = NULL);
	virtual ~CMesh();
	virtual void ReleaseUploadBuffers();

	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
	UINT GetType() { return(m_nType); }
};
class CTexturedModelingMesh : public CMesh
{
protected:
	char m_pstrMeshName[256] = { 0 };

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	std::vector<XMFLOAT2> m_pxmf2TextureCoords0;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord0Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord0UploadBuffer = NULL;

	std::vector<XMFLOAT2> m_pxmf2TextureCoords1;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord1Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord1UploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3Tangents;
	ComPtr<ID3D12Resource> m_pd3dTangentBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTangentUploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3BiTangents;
	ComPtr<ID3D12Resource> m_pd3dBiTangentBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dBiTangentUploadBuffer = NULL;
public:
	CTexturedModelingMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual ~CTexturedModelingMesh() {};

	virtual void ReleaseUploadBuffers() {};
	void LoadTexturedMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};


class CRawFormatImage
{
protected:
	BYTE* m_pRawImagePixels = NULL;

	int							m_nWidth;
	int							m_nLength;

public:
	CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY = false);
	~CRawFormatImage(void);

	BYTE GetRawImagePixel(int x, int z) { return(m_pRawImagePixels[x + (z * m_nWidth)]); }
	void SetRawImagePixel(int x, int z, BYTE nPixel) { m_pRawImagePixels[x + (z * m_nWidth)] = nPixel; }

	BYTE* GetRawImagePixels() { return(m_pRawImagePixels); }

	int GetRawImageWidth() { return(m_nWidth); }
	int GetRawImageLength() { return(m_nLength); }
};


class CHeightMapImage : public CRawFormatImage
{
protected:
	XMFLOAT3					m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetHeightMapNormal(int x, int z);
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int								m_nWidth;
	int								m_nLength;
	XMFLOAT3						m_xmf3Scale;

protected:

	std::vector<XMFLOAT4> m_pxmf4Colors;
	std::vector<XMFLOAT2> m_pxmf2TextureCoords0;
	std::vector<XMFLOAT2> m_pxmf2TextureCoords1;


	ComPtr<ID3D12Resource> m_pd3dColorBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dColorUploadBuffer = NULL;

	ComPtr<ID3D12Resource> m_pd3dTextureCoord0Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord0UploadBuffer = NULL;

	ComPtr<ID3D12Resource> m_pd3dTextureCoord1Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord1UploadBuffer = NULL;

public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CHeightMapGridMesh();

	virtual void ReleaseUploadBuffers();
	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void* pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);
};
