#pragma once
#include "Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Light.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\ParticleObject.h"
#include "..\Shader\ParticleShader.h"

class CMainTMPScene : public CScene
{
private:
	std::vector<std::unique_ptr<CGameObject>> m_pObjects;
	std::unique_ptr<CLight> m_pLight;
	std::unique_ptr<CSplatTerrain> m_pTerrain;
	std::unique_ptr<CShader> m_pTerrainShader;
	std::unique_ptr<CShader> m_pDepthRenderShader;
	std::vector<std::unique_ptr<CGameObject>> m_pBillBoardObjects;
	std::unique_ptr<CBillBoardObjectShader> m_pBillBoardObjectShader;

	std::vector<std::unique_ptr<CParticleObject>> m_ppParticleObjects;
	std::shared_ptr<CParticleShader> m_pParticleShader;

public:
	CMainTMPScene() {}
	virtual ~CMainTMPScene() {}

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRenderTarget();
};