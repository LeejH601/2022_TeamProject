#pragma once
#include "..\Global\stdafx.h"
#include "Animation.h"
#include "..\Global\Entity.h"

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
	bool bHit = false;
	
	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Texture;

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
	XMFLOAT4X4 GetWorld();
	CGameObject* GetParent() { return (m_pParent); }
	UINT GetMeshType();
	bool GetHit() { return bHit; }
	virtual BoundingBox GetBoundingBox() { return BoundingBox{}; }

	void SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate = false);
	void SetShader(std::shared_ptr<CShader> pShader, std::shared_ptr<CTexture> pTexture = NULL);
	void SetMaterial(int nMaterial, std::shared_ptr<CMaterial> pMaterial);
	void SetTexture(std::shared_ptr<CTexture> pTexture);
	void SetMesh(std::shared_ptr<CMesh> pMesh) { m_pMesh = pMesh; }

	void SetScale(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetTransform(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }
	void SetWorld(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }
	virtual void SetHit(CGameObject* pHitter) { bHit = true; }
	virtual void SetNotHit() { bHit = false; }
	
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f));

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
	CGameObject* FindFrame(const char* pstrFrameName);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();

	virtual void PrepareAnimate() {}
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual bool CheckCollision(CGameObject* pTargetObject) { return false; };

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, int* pnSkinnedMeshes);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile);
	
	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);
	void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CBoundingBoxObject : public CGameObject
{
public:
	CBoundingBoxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents);
	virtual ~CBoundingBoxObject();

	void PrepareRender();
	void SetParent(CGameObject* pObject) { m_pParent = pObject; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
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
class CKightRootRollBackAnimationController : public CAnimationController
{
public:
	CKightRootRollBackAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CKightRootRollBackAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CKightNoMoveRootAnimationController : public CAnimationController
{
public:
	CKightNoMoveRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CKightNoMoveRootAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject);
};
class CKnightObject : public CGameObject, public IEntity
{
private:
	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoudningBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CKnightObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CKnightObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class COrcObject : public CGameObject, public IEntity
{
private:
	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoudningBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~COrcObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual bool CheckCollision(CGameObject* pTargetObject) {
		bool flag = false;
		if (pTargetObject)
		{
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox)) {
				pTargetObject->SetDisable();
				flag = true;
			}
		}
		return flag;
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGoblinObject : public CGameObject
{
private:
	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoudningBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CGoblinObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual bool CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			bool flag = false;
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox)) {
				pTargetObject->SetDisable();
				flag = true;
			}
			return flag;
		}
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkeletonObject : public CGameObject
{
private:
	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoudningBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CSkeletonObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual bool CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			bool flag = false;
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox)) {
				pTargetObject->SetDisable();
				flag = true;
			}
			return flag;
		}
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CLionObject : public CGameObject
{
public:
	CLionObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CLionObject();

	virtual void Animate(float fTimeElapsed);
};