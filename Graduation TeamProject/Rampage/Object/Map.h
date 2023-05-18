#pragma once
#include "..\Global\stdafx.h"

class CShader;
class CSplatTerrain;
class CGameObject;
class CMap {
	std::unique_ptr<CShader> m_pTerrainShader;
	std::unique_ptr<CSplatTerrain> m_pTerrain;
	std::vector<std::unique_ptr<CGameObject>> m_pMapObjects;
public:
	CMap() { }
	~CMap() { }
	std::vector<std::unique_ptr<CGameObject>>& GetMapObjects() { return m_pMapObjects; }
	std::unique_ptr<CSplatTerrain>& GetTerrain() { return m_pTerrain; }

	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void UpdateObjectArticulation();
	void Update(float fTimeElapsed);
	void RenderTerrain(ID3D12GraphicsCommandList* pd3dCommandList);
	void RenderMapObjects(ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
};
