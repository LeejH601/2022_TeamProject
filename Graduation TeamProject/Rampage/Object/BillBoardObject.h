#include "../Object/Object.h"
#include "../Shader/BillBoardObjectShader.h"
class CBillBoardObject : public CGameObject
{
public:
	CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CBillBoardObjectShader* pShader); // 이미지 이름, 
	virtual ~CBillBoardObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};