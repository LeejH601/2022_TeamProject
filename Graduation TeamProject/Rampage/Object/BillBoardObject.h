#pragma once
#include "../Object/Object.h"
#include "..\Object\Terrain.h"
#include "../Shader/BillBoardObjectShader.h"


struct CB_FRAMEWORK_INFO
{
	float					m_fCurrentTime;
	float					m_fElapsedTime;
	float					m_fLifeTime = 0.f;
	bool					m_bEmitter = false;

	XMUINT2					m_iTextureCoord = XMUINT2(0, 0);
	UINT					m_iTextureIndex = 0;
	UINT					m_nParticleType = 0;

	XMFLOAT3				m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	float					m_fSpeed = 1.0f;

	XMFLOAT3				m_xmf3Color = XMFLOAT3(0.0f, 0.f, 1.0f);
	int						m_nFlareParticlesToEmit = 30;

	XMFLOAT2				m_fSize = XMFLOAT2(1.f, 1.f);
};

#define MAX_DETAILS_INSTANCES_ONE_DRAW_CALL 4000
struct CB_DETAIL_INFO
{
	XMFLOAT4 m_xmf4SizeOffset;
	XMFLOAT3 m_xmf3WorldPositions[MAX_DETAILS_INSTANCES_ONE_DRAW_CALL];
};


class CBillBoardObject : public CGameObject
{
public:
	CBillBoardObject() = default;
	CBillBoardObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, bool bBillBoard); // 이미지 이름, 
	virtual ~CBillBoardObject();
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void SetEnable(bool bEnable);
	virtual bool& GetEnable();

protected:
	CGameObject*					m_pTarget = NULL;
	bool							m_bEnable = false;
	float							m_fSpeed = 5.f;
	float							m_fTime = 0.0f;
	float							m_fAlpha = 1.f;
	XMFLOAT2						m_fSize = XMFLOAT2(15.f, 15.f);
	float							m_fLifeTime = 5.f;
	XMFLOAT3						m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	bool							m_bStart = false;

protected:
	ComPtr<ID3D12Resource>			m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO*				m_pcbMappedFrameworkInfo = NULL;
};
 // 일단 빌보드에는 시간값을 적용x(현재 쉐이더에 바닥 이펙트를 수정하면 적용 가능)
class CMultiSpriteObject : public CBillBoardObject
{
public:
	CMultiSpriteObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nRows, int nCols, float fSize, bool bBillBoard, float fSpeed = 5.f);
	virtual ~CMultiSpriteObject();

	virtual void Animate(float fTimeElapsed);
	virtual void AnimateRowColumn(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera);
	virtual void SetEnable(bool bEnable);

	void SetTotalRowColumn(int iTotalRow, int iTotalColumn);
	void SetColor(XMFLOAT3 fColor);
	void SetSize(XMFLOAT2 fSize);
	void SetSpeed(float fSpeed);
	void SetStartAlpha(float fAlpha);
	void SetLifeTime(float fLifeTime);
	void SetStart(bool bStart);
	bool& GetAnimation();
	bool m_bAnimation = true;
protected:
	int								m_iTotalRow = 1;
	int								m_iTotalCol = 1;

	int 							m_iCurrentRow = 0;
	int 							m_iCurrentCol = 0;
	//int								m_nMaxCol = 1; // 최대 출력 열 지정

	float m_fAccumulatedTime = 0.0f;
	float interval;
};

class CTerrainSpriteObject : public CMultiSpriteObject
{
public:
	CTerrainSpriteObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nRows, int nCols, float fSize, float fSpeed = 5.f);
	virtual ~CTerrainSpriteObject();
	virtual void Animate(CHeightMapTerrain* pTerrain, float fTimeElapsed); // 해당 오브젝트를 터레인 위에 위치시키는 함수
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);

	virtual void SetEnable(bool bEnable);

	void SetType(TerrainSpriteType eType);

private:
	float m_fDeltaSize = 0.f;
	TerrainSpriteType m_eTerrainSpriteType = TerrainSpriteType::TERRAINSPRITE_CROSS_FADE;
};


class CDetailObject : public CBillBoardObject 
{
public:
	CDetailObject() = default;
	CDetailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<CShader> pShader, void* pContext);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime);
	virtual ~CDetailObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

protected:
	std::vector<ComPtr<ID3D12Resource>> 		m_ppd3dcbDetailInfo;
	std::vector<CB_DETAIL_INFO*>			m_pcbMappedDetailInfo;

	int nDetails = 0;
	std::vector<XMFLOAT3> m_xmf3DetailPositions;
	std::vector<XMFLOAT3> m_xmf3DetailSizesAndWindOffset;
	std::vector<XMFLOAT3> m_xmf3DetailColors;
	std::vector<XMFLOAT3> m_xmf3DetailNormals;

	ComPtr<ID3D12Resource> m_pd3dPositionBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dPositionUploadBuffer = NULL;

	ComPtr<ID3D12Resource> m_pd3dSizeBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dSizeUploadBuffer = NULL;

	ComPtr<ID3D12Resource> m_pd3dColorBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dColorUploadBuffer = NULL;

	ComPtr<ID3D12Resource> m_pd3dNormalBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dNormalUploadBuffer = NULL;

	UINT m_nVertexBufferViews = 0;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> m_pd3dVertexBufferViews;

	D3D12_PRIMITIVE_TOPOLOGY        m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	XMFLOAT2 m_xmf2Size;
	XMFLOAT2 m_xmf2Offset;

	int nDrawSetIndex = 0;
	int nDrawInstanceOffset;
	int nDrawInstanceRange;
};