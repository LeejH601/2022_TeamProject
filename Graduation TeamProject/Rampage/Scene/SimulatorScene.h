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
#include "..\Object\TextureManager.h"


class CSimulatorScene : public CScene
{
private:
	std::vector<std::unique_ptr<CGameObject>> m_pEnemys;
	std::unique_ptr<CPlayer> m_pMainCharacter;
	std::unique_ptr<CLight> m_pLight;

	std::unique_ptr<CShader> m_pTerrainShader;
	std::unique_ptr<CSplatTerrain> m_pTerrain;

	std::unique_ptr<CShader> m_pDepthRenderShader;

	std::unique_ptr<CParticleShader> m_pParticleShader;
	std::vector<std::unique_ptr<CGameObject>> m_pParticleObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pUpDownParticleObjects;

	std::vector<std::unique_ptr<CGameObject>> m_pSpriteAttackObjects;
	std::vector<std::unique_ptr<CGameObject>> m_pTerrainSpriteObject;

	std::unique_ptr<CBillBoardObjectShader> m_pBillBoardObjectShader;
	std::vector<std::unique_ptr<CGameObject>> m_pBillBoardObjects;

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
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection) { return false; }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() {}

	virtual void Update(float fTimeElapsed);
	virtual void UpdateObjects(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	virtual void HandleCollision(const CollideParams& params);

	void SetPlayerAnimationSet(int nSet);
	CTextureManager* GetTextureManager() { return m_pTextureManager.get(); }
};