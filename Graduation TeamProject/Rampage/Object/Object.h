#pragma once
#include "..\Global\stdafx.h"
#include "Animation.h"

#define MAX_FRAMENAME 64

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

struct MATERIAL
{
	XMFLOAT4 m_cAmbient;
	XMFLOAT4 m_cDiffuse;
	XMFLOAT4 m_cSpecular;
	XMFLOAT4 m_cEmissive;
};

class CMesh;
class CCamera;
class CShader;
class CTexture;
class CMaterial;

class CGameObject
{
public:
	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Texture;

	BoundingOrientedBox m_xmOOBB;
	std::shared_ptr<CMesh> m_pMesh;
	
	int	m_nMaterials = 0;
	std::vector<std::shared_ptr<CMaterial>> m_ppMaterials;

	CGameObject* m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;
public:
	std::unique_ptr<CAnimationController> m_pSkinnedAnimationController;

	CGameObject();
	CGameObject(int nMaterials);
	virtual ~CGameObject();

	char* GetFrameName() { return m_pstrFrameName; }
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4 GetTransform();
	CGameObject* GetParent() { return (m_pParent); }
	UINT GetMeshType();

	void SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate = false);
	void SetShader(std::shared_ptr<CShader> pShader, std::shared_ptr<CTexture> pTexture = NULL);
	void SetMaterial(int nMaterial, std::shared_ptr<CMaterial> pMaterial);
	void SetTexture(std::shared_ptr<CTexture> pTexture);
	void SetMesh(std::shared_ptr<CMesh> pMesh) { m_pMesh = pMesh; }

	void SetScale(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetTransform(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
	CGameObject* FindFrame(const char* pstrFrameName);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();

	virtual void PrepareAnimate() {}
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, int* pnSkinnedMeshes);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile);
	
	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);
	void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAngrybotObject : public CGameObject
{
public:
	CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CAngrybotObject();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CEagleObject : public CGameObject
{
public:
	CEagleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CEagleObject();

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CKnightObject : public CGameObject
{
public:
	CKnightObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CKnightObject();

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class COrcObject : public CGameObject
{
public:
	COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~COrcObject();

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGoblinObject : public CGameObject
{
public:
	CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CGoblinObject();

	virtual void Animate(float fTimeElapsed);
};