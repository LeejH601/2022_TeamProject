#pragma once
#include "../Object/Mesh.h"
#include "../Object/Object.h"
#include "../Object/BillBoardObject.h"
#include "../Global/Camera.h"

#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2

class CParticleShader;

struct CB_CURLNOISE_INFO
{
	float Dt;
	float Time;
	float FieldSpeed;
	float NoiseStrength;
	XMFLOAT3 FieldMainDirection;
	float ProgressionRate;
	XMFLOAT3 SpherePosition;
	float LengthScale;
	float SphereRadius;
};

class CParticleObject : public CGameObject
{
public:
	CParticleObject();
	CParticleObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType);
	virtual ~CParticleObject();


	std::shared_ptr<CTexture> m_pRandowmTexture = NULL;
	std::shared_ptr<CTexture> m_pRandowmValueOnSphereTexture = NULL;

	void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);

	virtual void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nPipelineState);
	virtual void StreamOutRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	virtual void DrawRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	virtual void OnPostRender();

	void SetSize(XMFLOAT2 fSize);
	void SetLifeTime(float fLifeTime);
	void SetStartAlpha(float fAlpha);
	void SetColor(XMFLOAT3 fColor);
	void SetEmitParticleN(int iParticleN);
	void SetMaxParticleN(int iMaxParticleN);
	void SetSpeed(float fSpeed);
	void SetParticleType(int iParticleType);
	int	 GetParticleType();
	void SetEmit(bool bEmit);
	void SetFieldSpeed(float fFieldSpeed) { m_fFieldSpeed = fFieldSpeed; };
	void SetNoiseStrength(float fNoiseStrength) { m_fNoiseStrength = fNoiseStrength; };
	void SetProgressionRate(float fProgressionRate) { m_fProgressionRate = fProgressionRate; };
	void SetLengthScale(float fLengthScale) { m_fLengthScale = fLengthScale; };
	void SetFieldMainDirection(XMFLOAT3 xmf3FieldMainDirection) { m_xmf3FieldMainDirection = xmf3FieldMainDirection; };

	void SetDirection(XMFLOAT3 xmf3Direction);
	XMFLOAT3 GetDirection();

	bool CheckCapacity();
protected:
	XMFLOAT2	m_fSize = XMFLOAT2(15.f, 15.f);
	float		m_fAlpha = 1.f;
	float		m_fLifeTime = 1.f;
	float		m_fTime = 0.f;
	XMFLOAT3	m_f3Color = XMFLOAT3(1.f, 1.f, 1.f);
	int			m_iEmitParticleN = 100;
	int			m_iMaxParticleN = m_iEmitParticleN;

	float		m_fSpeed = 10.f;
	int			m_iParticleType = 0;
	XMFLOAT3	m_xmf3Direction = XMFLOAT3(1.f, 1.f, 1.f);

	int			m_nVertices = 0;

	float m_fFieldSpeed;
	float m_fNoiseStrength;
	XMFLOAT3 m_xmf3FieldMainDirection;
	float m_fProgressionRate;
	float m_fLengthScale;

protected:
	ComPtr<ID3D12Resource>	m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO* m_pcbMappedFrameworkInfo = NULL;

	ComPtr<ID3D12Resource>	m_pd3dcbCurlNoiseInfo = NULL;
	CB_CURLNOISE_INFO* m_pcbMappedCurlNoiseInfo = NULL;
};

class CSmokeParticleObject : public CParticleObject
{
public:
	CSmokeParticleObject(LPCTSTR pszFileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType);
	virtual ~CSmokeParticleObject();
};