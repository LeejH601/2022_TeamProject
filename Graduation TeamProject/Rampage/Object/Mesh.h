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

	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
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
	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};