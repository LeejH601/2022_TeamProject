#pragma once
#include "../Object/Mesh.h"
#include "../Object/Object.h"
#include "../Object/BillBoardObject.h"
class CParticleShader;

class CParticleObject : public CGameObject
{
public:
	CParticleObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, bool bEnable = false);
	virtual ~CParticleObject();


	std::shared_ptr<CTexture> m_pRandowmTexture = NULL;
	std::shared_ptr<CTexture> m_pRandowmValueOnSphereTexture = NULL;

	void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);

	virtual void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nPipelineState);
	virtual void StreamOutRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	virtual void DrawRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	virtual void OnPostRender();

	bool& GetEnable();
	void SetEnable(bool bEnable);

	void SetSize(float fSize);
	void SetLifeTime(float fLifeTime);
	void SetAlpha(float fAlpha);
	void SetColor(XMFLOAT3 fColor);
	void SetEmitParticleN(int iParticleN);
	void SetMaxParticleN(int iMaxParticleN);
	void SetSpeed(float fSpeed);

private:
	bool		m_bEnable = false;
	float		m_fSize = 0.8f;
	float		m_fAlpha = 1.f;
	float		m_fLifeTime = 3.f;
	float		m_fTime = 0.f;
	XMFLOAT3	m_f3Color = XMFLOAT3(1.f, 1.f, 1.f);
	int			m_iEmitParticleN = 100;
	int			m_iMaxParticleN = MAX_PARTICLES;
	float		m_fSpeed = 10.f;
protected:
	ComPtr<ID3D12Resource>	m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO* m_pcbMappedFrameworkInfo = NULL;
};
