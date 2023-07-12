#pragma once
#include <list>
#include "Scene.h"
#include "..\Global\Locator.h"
#include "..\Object\Object.h"
#include "..\Object\Map.h"
#include "..\Object\Light.h"
#include "..\Object\TextureManager.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\ParticleObject.h"
#include "..\Shader\ParticleShader.h"
#include "..\Shader\SkyBoxShader.h"
#include "..\Object\SkyBox.h"
#include "..\Shader\PostProcessShader.h"
#include "..\Shader\SunLightShader.h"
#include "..\Shader\HDRComputeShader.h"
#include "..\Shader\LensFlareShader.h"
#include "..\Shader\SwordTrailShader.h"
#include "..\Object\SwordTrailObject.h"
#include "..\Object\VertexPointParticleObject.h"
#include "..\Object\CollisionChecker.h"

enum class SCENE_PROCESS_TYPE
{
	NORMAL,
	CINEMATIC,
	WAITING
};

#define MAX_OBJECT 1000
#define MAX_WAIT_TIME 2.5f

struct DissolveParams {
	float dissolveThreshold[MAX_OBJECT];
};

struct SpawnInfo {
	int m_iGoblinNum = 0;
	int m_iEliteGolbinNum = 0;
	int m_iOrcNum = 0;
	int m_iEliteOrcNum = 0;
	int m_iSkeletonNum = 0;
	int m_iEliteSkeletonNum = 0;
	XMFLOAT3 m_xmf3StageCenterPos;
	float m_fMonsterSpawnRange;
};

struct StageInfo {
	std::vector<SpawnInfo> m_vSpawnInfo;
	float m_fTotalHP;
};

class CMainTMPScene : public CScene
{
private:
	SCENE_PROCESS_TYPE m_curSceneProcessType = SCENE_PROCESS_TYPE::NORMAL;
	float m_fWaitingTime;

	int m_iCursorHideCount = 0;
	RECT m_ScreendRect;

	bool m_bGameStart = false;
	float m_fGameStartTime;

	int m_iStageNum = 0;
	int m_iTotalMonsterNum = 0;

	float m_fCurrentTime = 0.f;
	std::map<int, StageInfo> m_StageInfoMap;

	std::unique_ptr<CollisionChecker> m_pCollisionChecker = NULL;
	std::vector<std::unique_ptr<CGameObject>> m_pEnemys;
	std::unique_ptr<CMap> m_pMap;

	std::vector<UINT> m_IObjectIndexs;
	std::unique_ptr<CLight> m_pLight;

	std::unique_ptr<CShader> m_pDepthRenderShader;
	std::unique_ptr<CShader> m_pUIObjectShader;
	std::vector<std::unique_ptr<CGameObject>> m_pUIObject;
	std::unique_ptr<CDetailObject> m_pDetailObject;

	std::unique_ptr<CTextureManager> m_pTextureManager = NULL;

	std::shared_ptr<CBreakScreenEffectShader> m_pBreakScreenShader = NULL;
	std::unique_ptr<CParticleShader> m_pParticleShader;
	std::unique_ptr<CSlashHitShader> m_pSlashHitShader;
	std::vector<std::unique_ptr<CGameObject>> m_pParticleObjects;
	//std::vector<std::unique_ptr<CGameObject>> m_pUpDownParticleObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pSpriteAttackObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pTerrainSpriteObject;
	std::unique_ptr<CGameObject> m_pTrailParticleObjects;
	
	std::unique_ptr<CGameObject> m_pSmokeObject;

	std::unique_ptr<CSunLightShader> m_pSunLightShader;

	std::unique_ptr<CLensFlareShader> m_pLensFlareShader;

	std::unique_ptr<CSwordTrailShader> m_pSwordTrailShader;
	std::vector<std::unique_ptr<CGameObject>> m_pSwordTrailObjects;
	std::unique_ptr<CVertexPointParticleObject> m_pVertexPointParticleObject;
	std::unique_ptr<CParticleObject> m_pSlashHitObjects;

	std::unique_ptr<CSkyBoxShader> m_pSkyBoxShader;
	std::unique_ptr<CSkyBox> m_pSkyBoxObject;

	std::unique_ptr<CCamera> m_pFloatingCamera = NULL;
	std::unique_ptr<CCamera> m_pMainSceneCamera = NULL;
	std::unique_ptr<CCamera> m_pCinematicSceneCamera = NULL;

	std::vector<std::unique_ptr<CCamera>> m_vCinematicCameraLocations;

	CCamera* m_pCurrentCamera = NULL;

	POINT m_ptOldCursorPos;

	MOUSE_CUROSR_MODE m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::THIRD_FERSON_MODE;

	DissolveParams* m_pcbMappedDisolveParams = nullptr;
	ComPtr<ID3D12Resource> m_pd3dcbDisolveParams = nullptr;

	std::list<RegisterArticulationParams> m_lRequestObjects;
	std::list<RegisterArticulationSleepParams> m_lSleepObjects;
public:
	CMainTMPScene();
	virtual ~CMainTMPScene() {}

	void HandleMonsterDeadMessage();
	void HandlePlayerDeadMessage();
	void AdvanceStage();
	
	virtual void SetPlayer(CGameObject* pPlayer);

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateComputeRootSignature(ID3D12Device* pd3dDevice);

	virtual void UpdateObjectArticulation();
	virtual void RequestRegisterArticulation(RegisterArticulationParams param);
	virtual void RegisterArticulations();
	virtual void RequestSleepArticulation(RegisterArticulationSleepParams param);
	virtual void SleepArticulations();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual SCENE_RETURN_TYPE OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed);
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Enter(HWND hWnd);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	virtual void UIUpdate(CPlayer* pPlayer);
	virtual void LoadTextureObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void HandleCollision(const CollideParams& params);
	virtual void HandleOnGround(const OnGroundParams& params);

};