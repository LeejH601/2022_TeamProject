#pragma once
#include "..\Global\stdafx.h"
#include "Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Light.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"
#include "..\Object\Player.h"
#include "..\Object\Monster.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\ParticleObject.h"
#include "..\Shader\ParticleShader.h"

class CSimulatorScene : public CScene
{
private:
	std::vector<std::shared_ptr<CMonster>> m_pEnemys;
	std::unique_ptr<CPlayer> m_pMainCharacter;
	std::unique_ptr<CLight> m_pLight;
	std::unique_ptr<CSplatTerrain> m_pTerrain;
	std::unique_ptr<CShader> m_pTerrainShader;
	std::shared_ptr<CMultiSpriteObject> m_pBillBoardObject;
	std::unique_ptr<CBillBoardObjectShader> m_pBillBoardObjectShader;
	std::unique_ptr<CShader> m_pDepthRenderShader;
	std::shared_ptr<CParticleShader> m_pParticleObjectShader;
	std::shared_ptr<CParticleObject> m_pParticleObject;
public:
	DECLARE_SINGLE(CSimulatorScene);
	CSimulatorScene() {}
	virtual ~CSimulatorScene() {}

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
	virtual void CheckCollide();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRenderTarget();

	void SetPlayerAnimationSet(int nSet);
};