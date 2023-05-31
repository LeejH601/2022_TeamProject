#pragma once
#include "..\Global\stdafx.h"
#include "Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Light.h"
#include "..\Object\Map.h"
#include "..\Object\Player.h"
#include "..\Object\Monster.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\ParticleObject.h"
#include "..\Shader\ParticleShader.h"
#include "..\Object\TextureManager.h"
#include "..\Shader\SwordTrailShader.h"


class CSimulatorScene : public CScene
{
private:
	std::vector<std::unique_ptr<CGameObject>> m_pEnemys;
	std::unique_ptr<CPlayer> m_pMainCharacter;
	std::unique_ptr<CLight> m_pLight;

	std::unique_ptr<CMap> m_pMap;

	std::unique_ptr<CShader> m_pDepthRenderShader;

	std::unique_ptr<CParticleShader> m_pParticleShader;
	std::unique_ptr<CSlashHitShader> m_pSlashHitShader;
	std::vector<std::unique_ptr<CGameObject>> m_pParticleObjects;
	//std::vector<std::unique_ptr<CGameObject>> m_pUpDownParticleObjects;

	std::vector<std::unique_ptr<CGameObject>> m_pSpriteAttackObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pTerrainSpriteObject;

	std::unique_ptr<CSwordTrailShader> m_pSwordTrailShader;
	std::vector<std::unique_ptr<CGameObject>> m_pSwordTrailObjects;
	std::unique_ptr<CParticleObject> m_pSlashHitObjects;

	std::unique_ptr<CTextureManager> m_pTextureManager = NULL;
	std::unique_ptr<CCamera> m_pSimulaterCamera = NULL;
public:
	DECLARE_SINGLE(CSimulatorScene);
	CSimulatorScene() {}
	virtual ~CSimulatorScene() {}

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateComputeRootSignature(ID3D12Device* pd3dDevice);

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual SCENE_RETURN_TYPE OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection) { return SCENE_RETURN_TYPE::NONE; }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual void Update(float fTimeElapsed);
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	virtual void HandleCollision(const CollideParams& params);

	void ResetMonster();
	void SetPlayerAnimationSet(int nSet);
	CTextureManager* GetTextureManager() { return m_pTextureManager.get(); }
};