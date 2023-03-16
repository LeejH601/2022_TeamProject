#pragma once
#include "../Object/Object.h"
#include "../Shader/BillBoardObjectShader.h"

#define PARTICLE_TYPE_EMITTER		0
#define PARTICLE_TYPE_SHELL			1
#define PARTICLE_TYPE_FLARE01		2
#define PARTICLE_TYPE_FLARE02		3
#define PARTICLE_TYPE_FLARE03		4

#define SHELL_PARTICLE_LIFETIME		3.0f
#define FLARE01_PARTICLE_LIFETIME	2.5f
#define FLARE02_PARTICLE_LIFETIME	1.5f
#define FLARE03_PARTICLE_LIFETIME	2.0f


struct CB_FRAMEWORK_INFO
{
	float					m_fCurrentTime;
	float					m_fElapsedTime;
	float					m_fSpeed = 1.0f;
	int						m_nFlareParticlesToEmit = 30;
	XMFLOAT3				m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	int						m_nMaxFlareType2Particles = 15;
	XMFLOAT3				m_xmf3Color = XMFLOAT3(0.0f, 0.f, 1.0f);
	int						m_nParticleType = PARTICLE_TYPE_EMITTER;
	float					m_fLifeTime = SHELL_PARTICLE_LIFETIME;
	float					m_fSize = 0.5f;
	bool					m_bStart = true;
};
class CBillBoardObject : public CGameObject
{
public:
	CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, float fSize); // 이미지 이름, 
	virtual ~CBillBoardObject();
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	virtual void Set_Target(CGameObject* m_pTarget);
	virtual void Set_Enable(bool bEnable);
	virtual bool& GetEnable();

protected:
	CGameObject*					m_pTarget = NULL;
	bool							m_bEnable = false;
	float							m_fSpeed = 5.f;
	float							m_fTime = 0.0f;
	float							m_fAlpha = 1.f;
	float							m_fSize = 10.f;
	float							m_fLifeTime = 0.f;

protected:
	ComPtr<ID3D12Resource>			m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO*				m_pcbMappedFrameworkInfo = NULL;
};

class CMultiSpriteObject : public CBillBoardObject
{
public:
	CMultiSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSize, float fSpeed = 5.f);
	virtual ~CMultiSpriteObject();

	virtual void Animate(float fTimeElapsed);
	virtual void AnimateRowColumn(float fTimeElapsed);
	virtual void SetEnable(bool bEnable);

	void SetSize(float fSize);
	void SetSpeed(float fSpeed);
	void SetAlpah(float fAlpha);
	bool& GetAnimation();
	bool							m_bAnimation = true;
protected:
	int 							m_nRow = 0;
	int 							m_nCol = 0;

};
