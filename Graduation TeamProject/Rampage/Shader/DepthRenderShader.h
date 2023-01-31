#pragma once
#include "Shader.h"

struct TOOBJECTSPACEINFO
{
	XMFLOAT4X4						m_xmf4x4ToTexture;
	XMFLOAT4						m_xmf4Position;
};

struct TOLIGHTSPACES
{
	TOOBJECTSPACEINFO				m_pToLightSpaces[MAX_LIGHTS];
};

struct LIGHT;
class CTexture;
class CCamera;
class CGameObject;
class CDepthRenderShader : public CShader
{
private:
	LIGHT* m_pLights = NULL;
	std::vector<std::unique_ptr<CGameObject>>* m_pObjects;
	CGameObject* m_pTerrain;

	std::unique_ptr<TOLIGHTSPACES> m_pToLightSpaces;

	ComPtr<ID3D12Resource> m_pd3dcbToLightSpaces = NULL;
	TOLIGHTSPACES* m_pcbMappedToLightSpaces = NULL;

	std::unique_ptr<CTexture> m_pDepthTexture = NULL;
	std::unique_ptr<CCamera> m_ppDepthRenderCameras[MAX_DEPTH_TEXTURES];

	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dRtvCPUDescriptorHandles[MAX_DEPTH_TEXTURES];

	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap = NULL;
	ComPtr<ID3D12Resource> m_pd3dDepthBuffer = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;

	XMMATRIX						m_xmProjectionToTexture;
public:
	DECLARE_SINGLE(CDepthRenderShader);
	CDepthRenderShader();
	virtual ~CDepthRenderShader();

	void SetObjects(std::vector<std::unique_ptr<CGameObject>>* pObjects) { m_pObjects = pObjects; }
	void SetTerrain(CGameObject* pTerrain) { m_pTerrain = pTerrain; }
	void SetLight(LIGHT* pLights) { m_pLights = pLights; }
	CTexture* GetDepthTexture() { return m_pDepthTexture.get(); }

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, float fTimeElapsed);

	void PrepareShadowMap(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	void UpdateDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList);
};