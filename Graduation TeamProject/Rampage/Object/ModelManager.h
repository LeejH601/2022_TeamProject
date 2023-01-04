#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Object.h"

class CLoadedModelInfo;
class CModelManager
{
private:
	std::vector<std::unique_ptr<CLoadedModelInfo>> vModels;
public:
	DECLARE_SINGLE(CModelManager);
	CModelManager() {}
	~CModelManager() {}
	void LoadModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName);
};