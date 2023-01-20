#pragma once
#include "..\Global\stdafx.h"

class CGameObject;
class CSkinnedMesh;
class CAnimationSets;
class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { }
	~CLoadedModelInfo();

	std::string pFilePath;
	std::shared_ptr<CGameObject> m_pModelRootObject = NULL;

	int m_nSkinnedMeshes = 0;
	std::vector<CSkinnedMesh*> m_ppSkinnedMeshes; //[SkinnedMeshes], Skinned Mesh Cache

	std::shared_ptr<CAnimationSets> m_pAnimationSets = NULL;
public:
	void PrepareSkinning();
};

class CModelManager
{
public:
	std::vector<std::unique_ptr<CLoadedModelInfo>> vModels;
public:
	DECLARE_SINGLE(CModelManager);
	CModelManager() {}
	~CModelManager() {}
	void LoadModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName);
	CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName);
	void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel);
	CLoadedModelInfo* GetModelInfo(const char* pstrFileName);
};