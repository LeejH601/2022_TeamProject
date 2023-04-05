#pragma once
#include <list>
#include "Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Light.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"
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

#define MAX_OBJECT 1000
struct DissolveParams {
	float dissolveThreshold[MAX_OBJECT];
};


class CMainTMPScene : public CScene
{
private:
	std::vector<std::unique_ptr<CGameObject>> m_pObjects;
	std::vector<UINT> m_IObjectIndexs;
	std::unique_ptr<CLight> m_pLight;
	std::unique_ptr<CSplatTerrain> m_pTerrain;
	std::unique_ptr<CShader> m_pTerrainShader;
	std::unique_ptr<CShader> m_pDepthRenderShader;

	std::unique_ptr<CTextureManager> m_pTextureManager = NULL;

	std::unique_ptr<CParticleShader> m_pParticleShader;
	std::vector<std::unique_ptr<CGameObject>> m_pParticleObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pSpriteAttackObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pTerrainSpriteObject;
	
	std::unique_ptr<CGameObject> m_pSmokeObject;

	std::unique_ptr<CBillBoardObjectShader> m_pBillBoardObjectShader;
	std::vector<std::unique_ptr<CGameObject>> m_pBillBoardObjects;

	std::unique_ptr<CSunLightShader> m_pSunLightShader;

	std::unique_ptr<CLensFlareShader> m_pLensFlareShader;

	std::unique_ptr<CSkyBoxShader> m_pSkyBoxShader;
	std::unique_ptr<CSkyBox> m_pSkyBoxObject;

	std::unique_ptr<CCamera> m_pFloatingCamera = NULL;
	std::unique_ptr<CCamera> m_pMainSceneCamera = NULL;
	CCamera* m_pCurrentCamera = NULL;

	DissolveParams* m_pcbMappedDisolveParams = nullptr;
	ComPtr<ID3D12Resource> m_pd3dcbDisolveParams = nullptr;

	std::list<RegisterArticulationParams> m_lRequestObjects;
public:
	CMainTMPScene() {}
	virtual ~CMainTMPScene() {}
	
	virtual void SetPlayer(CGameObject* pPlayer);

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateComputeRootSignature(ID3D12Device* pd3dDevice);

	virtual void UpdateObjectArticulation() {
		for (int i = 0; i < m_pObjects.size(); ++i) {
			m_pObjects[i]->updateArticulationMatrix();
		}
	}
	virtual void RequestRegisterArticulation(RegisterArticulationParams param);
	virtual void RegisterArticulations();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	void LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
	virtual void HandleCollision(const CollideParams& params);
};