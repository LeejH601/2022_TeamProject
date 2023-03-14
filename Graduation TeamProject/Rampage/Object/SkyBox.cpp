#include "Texture.h"
#include "..\Global\Camera.h"
#include "Mesh.h"
#include "..\Shader\SkyBoxShader.h"
#include "SkyBox.h"


CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, std::shared_ptr<CTexture> pSkyBoxTexture)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();


	m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform,XMMatrixRotationRollPitchYaw(0.0f, -PI/2, 0.0f) );

	std::shared_ptr<CSkyBoxMesh> pSkyBoxMesh = std::make_shared<CSkyBoxMesh>(pd3dDevice, pd3dCommandList, 200.0f, 200.0f, 200.0f);
	SetMesh(pSkyBoxMesh);

	SetTexture(pSkyBoxTexture);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	static bool isSetSkyBoxPos = false;
	CGameObject::Render(pd3dCommandList, pCamera);
	//if (!isSetSkyBoxPos) {
		XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
		SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
		isSetSkyBoxPos = true;
	//}
}
