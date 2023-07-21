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

#define MAX_OBJECT 1000
struct DissolveDummyParams {
	float dissolveThreshold[MAX_OBJECT];
};
class CSimulatorScene : public CScene
{
private:
	bool m_bAutoReset;
	int m_CurrentMonsterNum;
	int m_AutoResetCount;
	MONSTER_TYPE m_CurrentMonsterType;

	std::vector<std::unique_ptr<CGameObject>> m_pEnemys;
	std::unique_ptr<CPlayer> m_pMainCharacter;
	std::unique_ptr<CLight> m_pLight;

	std::unique_ptr<CMap> m_pMap;

	std::unique_ptr<CShader> m_pDepthRenderShader;

	std::unique_ptr<CParticleShader> m_pParticleShader;
	std::unique_ptr<CSlashHitShader> m_pSlashHitShader;
	std::vector<std::unique_ptr<CGameObject>> m_pParticleObjects;
	//std::vector<std::unique_ptr<CGameObject>> m_pUpDownParticleObjects;

	std::unique_ptr<CGameObject> m_pTrailParticleObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pSpriteAttackObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pTerrainSpriteObject;

	std::unique_ptr<CSwordTrailShader> m_pSwordTrailShader;
	std::vector<std::unique_ptr<CGameObject>> m_pSwordTrailObjects;
	std::unique_ptr<CParticleObject> m_pSlashHitObjects;

	std::unique_ptr<CTextureManager> m_pTextureManager = NULL;
	std::unique_ptr<CCamera> m_pSimulaterCamera = NULL;

	DissolveDummyParams* m_pcbMappedDisolveParams = nullptr;
	ComPtr<ID3D12Resource> m_pd3dcbDisolveParams = nullptr;
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

	virtual void Enter(HWND hWnd);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	virtual void HandleCollision(const CollideParams& params);

	void ResetMonster(int index = 0, XMFLOAT3 xmf3Position = XMFLOAT3{ 47.5f, 0.0f, 50.0f });
	void SetPlayerAnimationSet(int nSet);
	void SetMonsterNum(int nMonsterNum);
	void SetAutoReset(bool bAutoReset) { m_bAutoReset = bAutoReset; }
	void SelectMonsterType(MONSTER_TYPE monster_type);
	void SpawnAndSetMonster();

	CTextureManager* GetTextureManager() { return m_pTextureManager.get(); }
	CCamera* GetCamera() { return m_pSimulaterCamera.get(); }
	bool GetAutoReset() { return m_bAutoReset; }
};