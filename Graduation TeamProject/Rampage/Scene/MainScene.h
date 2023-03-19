#pragma once
#include "Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Light.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\ParticleObject.h"
#include "..\Shader\ParticleShader.h"
#include "..\Shader\SkyBoxShader.h"
#include "..\Object\SkyBox.h"
#include "..\Shader\PostProcessShader.h"
#include "..\Shader\SunLightShader.h"
#include "..\Shader\HDRComputeShader.h"

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
	std::vector<std::unique_ptr<CGameObject>> m_pBillBoardObjects;
	std::unique_ptr<CBillBoardObjectShader> m_pBillBoardObjectShader;
	std::unique_ptr<CSunLightShader> m_pSunLightShader;

	std::vector<std::unique_ptr<CParticleObject>> m_ppParticleObjects;
	std::shared_ptr<CParticleShader> m_pParticleShader;

	std::unique_ptr<CSkyBoxShader> m_pSkyBoxShader;
	std::unique_ptr<CSkyBox> m_pSkyBoxObject;

	bool m_bTestKey = false;



private:
	DissolveParams* m_pcbMappedDisolveParams = nullptr;
	ComPtr<ID3D12Resource> m_pd3dcbDisolveParams = nullptr;

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

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void CheckCollide();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRenderTarget();

	void LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
};