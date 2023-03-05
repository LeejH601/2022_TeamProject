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

struct JointAxisDesc {
	physx::PxArticulationJointType::Enum type;

	bool eSwing1;
	bool eSwing2;
	bool eTWIST;

	physx::PxArticulationLimit eS1LImit;
	physx::PxArticulationLimit eS2LImit;
	physx::PxArticulationLimit eTLImit;

	physx::PxArticulationDrive eS1Drive;
	physx::PxArticulationDrive eS2Drive;
	physx::PxArticulationDrive eTDrive;
};

class CGameObject
{
public:
	bool bHit = false;

	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Texture;
	XMFLOAT4X4 m_xmf4x4Scale;

	std::shared_ptr<CMesh> m_pMesh;

	int	m_nMaterials = 0;
	std::vector<std::shared_ptr<CMaterial>> m_ppMaterials;

	CGameObject* m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;

	physx::PxActor* Rigid = nullptr;
	physx::PxArticulationReducedCoordinate* m_pArticulation;
	physx::PxArticulationCache* m_pArticulationCache;
	physx::PxU32 m_nArtiCache;
	std::vector<std::string> m_pArtiLinkNames;
	std::vector<physx::PxArticulationLink*> m_pArticulationLinks;
	std::vector<XMFLOAT4X4> m_AritculatCacheMatrixs;

	bool m_bSimulateArticulate = false;
	bool m_bDissolved = false;
	float m_fDissolveThrethHold = 1.0f;
	float m_fDissolveTime;
	float m_fMaxDissolveTime;

public:
	std::unique_ptr<CAnimationController> m_pSkinnedAnimationController;

	CGameObject();
	CGameObject(int nMaterials);
	virtual ~CGameObject();

	virtual void SetRigidDynamic() {};
	virtual void SetRigidStatic() {};

	char* GetFrameName() { 
		return m_pstrFrameName; 
	}
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
	void ChangeTexture(std::shared_ptr<CTexture> pTexture);
	void SetMesh(std::shared_ptr<CMesh> pMesh) { m_pMesh = pMesh; }

	virtual void SetScale(float x, float y, float z);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);
	void SetTransform(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }
	void SetWorld(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }
	virtual void SetHit(CGameObject* pHitter) { bHit = true; }
	virtual void SetNotHit() { bHit = false; }
	
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f));

	void updateArticulationMatrix();
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> m_pArtiLinkNames, std::vector<physx::PxArticulationLink*> m_pArticulationLinks, float scale = 1.0f);
	virtual void UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> m_pArtiLinkNames, std::vector<XMFLOAT4X4>& m_AritculatCacheMatrixs, float scale = 1.0f);
	void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
	CGameObject* FindFrame(const char* pstrFrameName);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();

	virtual void PrepareAnimate() {}
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void Update(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual bool CheckCollision(CGameObject* pTargetObject) { return false; };

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, int* pnSkinnedMeshes);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile);

	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);
	void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

	void CreateArticulation(physx::PxArticulationReducedCoordinate* articulation, physx::PxArticulationLink* Parent, const physx::PxTransform& pos);
	physx::PxTransform MakeTransform(XMFLOAT4X4& xmf44, float scale);
	void SetJoint(physx::PxArticulationJointReducedCoordinate* joint, JointAxisDesc& JointDesc);
	physx::PxArticulationLink* SetLink(physx::PxArticulationReducedCoordinate* articulation, physx::PxArticulationLink* p_link, physx::PxTransform& parent, physx::PxTransform& child, float meshScale = 1.0f);
	virtual void CreateArticulation(float meshScale = 1.0f);
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
	BoundingBox m_TransformedBodyBoundingBox;
	BoundingBox m_TransformedWeaponBoundingBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;

public:
	CKnightObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CKnightObject();

	virtual void SetRigidDynamic();
	

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoundingBox; }
	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CLionObject : public CGameObject
{
public:
	CLionObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CLionObject();

	virtual void Animate(float fTimeElapsed);
};