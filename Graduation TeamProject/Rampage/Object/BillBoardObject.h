#include "../Object/Object.h"
#include "../Shader/BillBoardObjectShader.h"
class CBillBoardObject : public CGameObject
{
public:
	CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader); // 이미지 이름, 
	virtual ~CBillBoardObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
};

class CMultiSpriteObject : public CBillBoardObject
{
public:
	CMultiSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSpeed = 1.f);
	virtual ~CMultiSpriteObject();

	virtual void Animate(float fTimeElapsed);
	virtual void AnimateRowColumn(float fTimeElapsed);
protected:
	int 							m_nRow = 0;
	int 							m_nCol = 0;

	int 							m_nRows = 1;
	int 							m_nCols = 1;

	float							m_fSpeed = 0.1f;
	float							m_fTime = 0.0f;

};
