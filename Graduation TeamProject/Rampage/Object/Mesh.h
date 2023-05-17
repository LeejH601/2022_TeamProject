#pragma once
#include "..\Global\stdafx.h"

#define VERTEXT_POSITION				0x01
#define VERTEXT_COLOR					0x02
#define VERTEXT_NORMAL					0x04
#define VERTEXT_TANGENT					0x08
#define VERTEXT_TEXTURE_COORD0			0x10
#define VERTEXT_TEXTURE_COORD1			0x20
#define VERTEXT_BONE_INDEX_WEIGHT		0x1000
class CGameObject;
class CMesh
{
public:
	char m_pstrMeshName[64] = { 0 };
protected:
	UINT m_nType = 0x00;
	UINT m_nVertices = 0;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

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
	virtual ~CMesh();
	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState) {};
	virtual void PostRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState) {};
	virtual int OnPostRender(int nPipelineState) {return m_nVertices;};
	std::vector<XMFLOAT3>& GetVertexs() { return m_pxmf3Positions; };
	std::vector<UINT>& GetIndices() { return m_ppnSubSetIndices[0]; };

	UINT GetType() { return(m_nType); }
	UINT GetVertices() { return(m_nVertices); }
	XMFLOAT3 GetBoundingCenter() { return(m_xmf3AABBCenter); }
	XMFLOAT3 GetBoundingExtent() { return(m_xmf3AABBExtents); }
};
class CTexturedModelingMesh : public CMesh
{
protected:
	char m_pstrMeshName[256] = { 0 };

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
	CTexturedModelingMesh() {}
	CTexturedModelingMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual ~CTexturedModelingMesh() {}

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset);
	virtual void ReleaseUploadBuffers() {}
	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};
#define SKINNED_ANIMATION_BONES		128
class CSkinnedMesh : public CTexturedModelingMesh
{
public:
	int	m_nBonesPerVertex = 4;

	std::vector<XMINT4> m_pxmn4BoneIndices;
	std::vector<XMFLOAT4> m_pxmf4BoneWeights;

	ComPtr<ID3D12Resource> m_pd3dBoneIndexBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dBoneIndexBufferView;

	ComPtr<ID3D12Resource> m_pd3dBoneWeightBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dBoneWeightBufferView;

	int	m_nSkinningBones = 0;

	std::vector<std::string> m_ppstrSkinningBoneNames;
	std::vector<CGameObject*> m_ppSkinningBoneFrameCaches; //[m_nSkinningBones]

	std::vector<XMFLOAT4X4> m_pxmf4x4BindPoseBoneOffsets; //Transposed

	ComPtr<ID3D12Resource> m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL;

	ComPtr<ID3D12Resource> m_pd3dcbSkinningBoneTransforms = NULL; //Pointer Only
	XMFLOAT4X4* m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;
public:
	CSkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual ~CSkinnedMesh() {};

	void PrepareSkinning(CGameObject* pModelRootObject);
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers() {};
	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};

struct TangentEdges
{
	XMFLOAT3 e0;
	XMFLOAT3 e1;
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
	void SetRawImagePixel(int x, int z, BYTE nPixel) { 
		m_pRawImagePixels[x + (z * m_nWidth)] = nPixel; 
	}

	BYTE* GetRawImagePixels() { return(m_pRawImagePixels); }

	int GetRawImageWidth() { return(m_nWidth); }
	int GetRawImageLength() { return(m_nLength); }

	BYTE* GetAllPixels() { return m_pRawImagePixels; };
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
	TangentEdges GetHeightMapTangent(int x, int z);
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
	CHeightMapGridMesh() : CMesh() {};
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CHeightMapGridMesh();

	virtual void ReleaseUploadBuffers();
	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void* pContext, bool SampleByImage = false);
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);
};
//-------------------------------------------------------------------
class CBoundingBoxMesh : public CMesh
{
public:
	CBoundingBoxMesh() {};
	CBoundingBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents);
	virtual ~CBoundingBoxMesh();
};

class CSplatGridMesh : public CHeightMapGridMesh
{
protected:
	std::vector<XMFLOAT3> m_pxmf3Normals;
	ComPtr<ID3D12Resource> m_pd3dNormalBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dNormalUploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3Tangents;
	ComPtr<ID3D12Resource> m_pd3dTangentBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTangentUploadBuffer = NULL;

public:
	CSplatGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CSplatGridMesh();
};
//-------------------------------------------------------------------
class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT2						m_xmf2Size;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f), m_xmf2Size = XMFLOAT2(0.f, 0.f); }

	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	CVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2Size) { m_xmf3Position = xmf3Position; m_xmf2Size = xmf2Size; }
	~CVertex() { }
};

class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};


class CTexturedRectMesh : public CMesh
{
public:
	std::vector<CTexturedVertex> m_pxmfTexturedVertexs;
public:
	CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

class CLensFlareVertex
{
public:
	XMFLOAT2						m_xmf2Size;
	UINT							m_iIndex;
public:
	CLensFlareVertex() { m_xmf2Size = XMFLOAT2(0.f, 0.f), m_iIndex = -1; }
	CLensFlareVertex(XMFLOAT2 xmf2Size, UINT iIndex)
	{
		m_xmf2Size = xmf2Size;
		m_iIndex = iIndex;
	}
	~CLensFlareVertex() { }
};

class CSpriteVertex
{
public:
	XMFLOAT2						m_xmf2Size;
	float							m_fLifetime;
	int								m_iBillBoard;
	//int							m_iTextureIndex;
public:
	CSpriteVertex() { m_xmf2Size = XMFLOAT2(0.f, 0.f), m_iBillBoard = true; m_fLifetime = 0.f; }
	CSpriteVertex(XMFLOAT2 xmf2Size, bool bBillBoard)
	{
		m_xmf2Size = xmf2Size;
		m_iBillBoard = bBillBoard;
		m_fLifetime = 0.f;
	}
	~CSpriteVertex() { }
};

class CTerrainMesh : public CMesh
{

public:
	CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float m_fSize);
	virtual ~CTerrainMesh();
};


class CSpriteAnimateVertex
{
public:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT2						m_xmf2Size;
	XMFLOAT2						m_xmf2TexCoord;
public:
	CSpriteAnimateVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f), m_xmf2Size = XMFLOAT2(0.f, 0.f), m_xmf2TexCoord = XMFLOAT2(0.f, 0.f); }
	CSpriteAnimateVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2Size, XMFLOAT2 xmf2TexCoord)
	{
		m_xmf3Position = xmf3Position;
		m_xmf2Size = xmf2Size;
		m_xmf2TexCoord = xmf2TexCoord;
	}
	~CSpriteAnimateVertex() { }
};

class CSpriteMesh : public CMesh
{

public:
	CSpriteMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float m_fSize, bool m_bBillBoard);
	virtual ~CSpriteMesh();
};

class CParticleVertex
{
public:
	XMFLOAT3						m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float							m_fLifetime = 0.0f;
	UINT							m_iType = 0;
	float							m_fEmitTime = 0.0f;
	UINT							m_iTextureIndex = 0;
	UINT							m_iTextureCoord[2];
public:
	CParticleVertex() { }
	~CParticleVertex() { }
};

struct ParticleEmitDataParam
{
	XMFLOAT3 m_xmf3EmitedPosition;
	float m_fEmitedSpeed;
	XMFLOAT3 m_xmf3EmitAxes;
	int m_nEmitNum;
	float m_fLifeTime;
	float							m_fEmitTime = 0.0f;
	UINT							m_iTextureIndex = 0;
	UINT							m_iTextureCoord[2];
};

#define MAX_PARTICLES				100000

//#define _WITH_QUERY_DATA_SO_STATISTICS

class CParticleMesh : public CMesh
{
public:
	CParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles);
	virtual ~CParticleMesh();

	bool								m_bEmit = true;
	bool								m_bInitialized = false;
	UINT								m_nMaxParticles = MAX_PARTICLES;
	UINT								m_nMaxParticle = MAX_PARTICLES;

	int m_ncreatedParticleNum = 0;

	ComPtr<ID3D12Resource>				m_pd3dVertexBuffer = NULL;
	ID3D12Resource*						m_pd3dStreamOutputBuffer = NULL;
	ID3D12Resource*						m_pd3dDrawBuffer = NULL;

	D3D12_RANGE m_d3dReadRange = { 0, 0 };
	UINT8* m_pBufferDataBegin = NULL;
	ComPtr<ID3D12Resource>				m_pd3dDrawUploadBuffer;

	ID3D12Resource*						m_pd3dDefaultBufferFilledSize = NULL;
	ID3D12Resource*						m_pd3dUploadBufferFilledSize = NULL;
	UINT64*								m_pnUploadBufferFilledSize = NULL;
#ifdef _WITH_QUERY_DATA_SO_STATISTICS
	ID3D12QueryHeap*					m_pd3dSOQueryHeap = NULL;
	ID3D12Resource*						m_pd3dSOQueryBuffer = NULL;
	D3D12_QUERY_DATA_SO_STATISTICS*		m_pd3dSOQueryDataStatistics = NULL;
#else
	ID3D12Resource*						m_pd3dReadBackBufferFilledSize = NULL;
#endif

	D3D12_STREAM_OUTPUT_BUFFER_VIEW		m_d3dStreamOutputBufferView;

	virtual void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size);
	virtual void CreateStreamOutputBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nMaxParticles);

	virtual void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nPipelineState);
	virtual void PostRender(ID3D12GraphicsCommandList* pd3dCommandList, UINT nPipelineState);

	virtual int OnPostRender(int nPipelineState);

	void EmitParticle(int emitType, ParticleEmitDataParam& param);
};


class CSkyBoxMesh : public CMesh
{
public:
	CSkyBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();
};