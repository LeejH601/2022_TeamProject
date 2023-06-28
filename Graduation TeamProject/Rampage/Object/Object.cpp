#include "Texture.h"
#include "Mesh.h"
#include "Object.h"
#include "ModelManager.h"
#include "..\Shader\Shader.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Global\Locator.h"
#include "..\Global\Timer.h"
#include "..\Global\Global.h"

CGameObject::CGameObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	//m_fDissolveThrethHold = 1.0f;
}
CGameObject::CGameObject(int nMaterials)
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials.resize(m_nMaterials);
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}
CGameObject::~CGameObject()
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->~CAnimationController();
}
void CGameObject::ReleaseShaderVariables()
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
	//pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_bRimLightEnable, 34);
	
	/*if (m_pcbMappedRimLightlInfo) {
		m_pcbMappedRimLightlInfo->m_fRimLightFactor = m_fRimLightFactor;
		m_pcbMappedRimLightlInfo->m_xmf3RimLightColor = m_xmf3RimLightColor;

		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbRimLightInfo->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(14, d3dGpuVirtualAddress);
	}*/
}
void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh)
		m_pMesh->ReleaseUploadBuffers();
}
void CGameObject::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}
void CGameObject::SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild)
	{
		pChild->m_pParent = this;
	}
}
void CGameObject::SetShader(std::shared_ptr<CShader> pShader, std::shared_ptr<CTexture> pTexture)
{
	if (!m_ppMaterials.data())
	{
		m_nMaterials = 1;
		m_ppMaterials.resize(m_nMaterials);
		std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
		pMaterial->SetShader(pShader);
		if (pTexture)
			pMaterial->SetTexture(pTexture);
		m_ppMaterials[0] = pMaterial;
	}
}
void CGameObject::SetMaterial(int nMaterial, std::shared_ptr<CMaterial> pMaterial)
{
	m_ppMaterials[nMaterial] = pMaterial;
}
void CGameObject::SetTexture(std::shared_ptr<CTexture> pTexture)
{
	if (!m_ppMaterials.data())
	{
		m_nMaterials = 1;
		m_ppMaterials.resize(m_nMaterials);
		std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
		pMaterial->SetTexture(pTexture);
		m_ppMaterials[0] = pMaterial;
	}
}
void CGameObject::SetTextureIndex(int iIndex)
{
	m_iTextureIndex = iIndex;
}
void CGameObject::ChangeTexture(std::shared_ptr<CTexture> pTexture)
{
	if (m_ppMaterials.data())
		m_ppMaterials[0]->SetTexture(pTexture);
	else
		SetTexture(pTexture);
}

void CGameObject::Update(float fTimeElapsed)
{
	if (!m_bEnable)
		return;
	Animate(fTimeElapsed);
}
void CGameObject::Animate(float fTimeElapsed)
{
	if (!m_bEnable)
		return;

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		UpdateShaderVariables(pd3dCommandList);
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, 0);
				if (b_UseTexture)
					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}
			// 여기서 메쉬의 렌더를 한다.
			m_pMesh->OnPreRender(pd3dCommandList);
			m_pMesh->Render(pd3dCommandList, i);
		}
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}
XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
}
XMFLOAT3 CGameObject::GetLook()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._31, m_xmf4x4Transform._32, m_xmf4x4Transform._33));
}
XMFLOAT3 CGameObject::GetUp()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._21, m_xmf4x4Transform._22, m_xmf4x4Transform._23));
}
XMFLOAT3 CGameObject::GetRight()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._11, m_xmf4x4Transform._12, m_xmf4x4Transform._13));
}
XMFLOAT4X4 CGameObject::GetTransform()
{
	return m_xmf4x4Transform;
}
XMFLOAT4X4 CGameObject::GetWorld()
{
	return m_xmf4x4World;
}
UINT CGameObject::GetMeshType()
{
	return((m_pMesh) ? m_pMesh->GetType() : 0x00);
}
void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
	XMStoreFloat4x4(&m_xmf4x4Scale, mtxScale);
}
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}
void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}
void CGameObject::SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4World._11 = mtxLookAt._11; m_xmf4x4World._12 = mtxLookAt._21; m_xmf4x4World._13 = mtxLookAt._31;
	m_xmf4x4World._21 = mtxLookAt._12; m_xmf4x4World._22 = mtxLookAt._22; m_xmf4x4World._23 = mtxLookAt._32;
	m_xmf4x4World._31 = mtxLookAt._13; m_xmf4x4World._32 = mtxLookAt._23; m_xmf4x4World._33 = mtxLookAt._33;
}
void CGameObject::updateArticulationMatrix()
{
	int index = 0;
	for (XMFLOAT4X4& world : m_AritculatCacheMatrixs) {
		physx::PxMat44 mat = m_pArticulationLinks[index++]->getGlobalPose();

		memcpy(&world, &mat, sizeof(XMFLOAT4X4));
		world = Matrix4x4::Multiply(XMMatrixScaling(m_xmf4x4Scale._11, m_xmf4x4Scale._11, m_xmf4x4Scale._11), world);
	}
}
void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> pArtiLinkNames, std::vector<physx::PxArticulationLink*> pArticulationLinks, float scale)
{
	std::string target = m_pstrFrameName;
	auto it = std::find(pArtiLinkNames.begin(), pArtiLinkNames.end(), target);
	int distance = std::distance(pArtiLinkNames.begin(), it);
	if (distance < pArtiLinkNames.size()) {

		physx::PxTransform transform = pArticulationLinks[distance]->getGlobalPose();
		//physx::PxTransform transform = m_pArticulationLinks[index]->getInboundJoint()->getChildPose();
		physx::PxMat44 World = physx::PxMat44(transform);

		memcpy(&m_xmf4x4World, &World, sizeof(XMFLOAT4X4));
		m_xmf4x4World = Matrix4x4::Multiply(XMMatrixScaling(scale, scale, scale), m_xmf4x4World);
		/*m_xmf4x4World._41 = World.column3.x;
		m_xmf4x4World._42 = World.column3.y;
		m_xmf4x4World._43 = World.column3.z;*/
	}
	else {
		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	}

	if (m_pSibling) m_pSibling->UpdateTransformFromArticulation(pxmf4x4Parent, pArtiLinkNames, pArticulationLinks, scale);
	if (m_pChild) m_pChild->UpdateTransformFromArticulation(&m_xmf4x4World, pArtiLinkNames, pArticulationLinks, scale);
}

void CGameObject::UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> pArtiLinkNames, std::vector<XMFLOAT4X4>& AritculatCacheMatrixs, float scale)
{
	std::string target = m_pstrFrameName;
	auto it = std::find(pArtiLinkNames.begin(), pArtiLinkNames.end(), target);
	int distance = std::distance(pArtiLinkNames.begin(), it);
	if (distance < pArtiLinkNames.size()) {
		m_xmf4x4World = AritculatCacheMatrixs[distance];
	}
	else {
		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	}

	if (m_pSibling) m_pSibling->UpdateTransformFromArticulation(pxmf4x4Parent, pArtiLinkNames, AritculatCacheMatrixs, scale);
	if (m_pChild) m_pChild->UpdateTransformFromArticulation(&m_xmf4x4World, pArtiLinkNames, AritculatCacheMatrixs, scale);
}

CGameObject* CGameObject::FindFrame(const char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if ((!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) && strlen(m_pstrFrameName) == strlen(pstrFrameName))
	{
		return(this);
	}
	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}
void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*m_fRimLightFactor = 0.15f;
	m_xmf3RimLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);*/

	//UINT ncbElementBytes = ((sizeof(VS_CB_RIMLIGHT_INFO) + 255) & ~255); //256의 배수
	//m_pd3dcbRimLightInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedRimLightlInfo, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_pd3dcbRimLightInfo->Map(0, NULL, (void**)&m_pcbMappedRimLightlInfo);
}
void CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, int* pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(pInFile);
			nTextures = ::ReadIntegerFromFile(pInFile);

			int length = ::ReadStringFromFile(pInFile, m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			std::shared_ptr<CSkinnedMesh> pMesh = std::make_shared<CSkinnedMesh>(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			std::shared_ptr<CSkinnedMesh> pSkinnedMesh = std::make_shared<CSkinnedMesh>(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					std::shared_ptr<CGameObject> pChild = std::make_shared<CGameObject>();
					pChild->LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, this, pInFile, pnSkinnedMeshes);
					if (pChild) SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
}
void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling && pParent) CGameObject::PrintFrameInfo(pGameObject->m_pSibling.get(), pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild.get(), pGameObject);
}
void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials.resize(m_nMaterials);
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	std::shared_ptr<CMaterial> pMaterial = NULL;
	std::shared_ptr<CTexture> pTexture = NULL;

	static int TextureLoadCnt = 3;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{

			/*if (TextureLoadCnt < 3) {
				for(int i =0;i< 3 - TextureLoadCnt;++i)
					CModelShader::GetInst()->DescriptorHandleMoveNext();
			}*/
			//TextureLoadCnt = 0;
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = std::make_shared<CMaterial>();
			pTexture = std::make_shared<CTexture>(8, RESOURCE_TEXTURE2D, 0, 1); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal 7:NoiseTexture
			//pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Object/Textures/Perlin9.dds", RESOURCE_TEXTURE2D, 7);
			pTexture->SetRootParameterIndex(0, 2);

			pMaterial->SetTexture(pTexture);
			SetMaterial(nMaterial, pMaterial);

			UINT nMeshType = GetMeshType();
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			//TextureLoadCnt++;
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		/*else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}*/
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 2)) {
				pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);

			}
			else {
				CModelShader::GetInst()->DescriptorHandleMoveNext();
			}
			pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Object/Textures/Grainy10.dds", RESOURCE_TEXTURE2D, 3);
			CModelShader::GetInst()->CreateShaderResourceView(pd3dDevice, pTexture.get(), 3);
		}
		/*else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}*/
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}
int CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcscmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}
void CGameObject::FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (CSkinnedMesh*)(m_pMesh.get());

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}
void CGameObject::CreateArticulation(physx::PxArticulationReducedCoordinate* articulation, physx::PxArticulationLink* Parent, const physx::PxTransform& pos)
{
	static int count = 0;
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("cnt : %d - %s \n"), ++count, m_pstrFrameName);
	OutputDebugString(pstrDebug);

	physx::PxMat44 mymatrix;
	XMFLOAT4X4 myTransform = m_xmf4x4Transform;
	XMFLOAT3 mypos(GetPosition());
	myTransform._41 = 0;
	myTransform._42 = 0;
	myTransform._43 = 0;
	memcpy(&mymatrix, &Matrix4x4::Inverse(myTransform), sizeof(physx::PxMat44));

	mymatrix.column3.x = mypos.x;
	mymatrix.column3.y = mypos.y;
	mymatrix.column3.z = mypos.z;

	physx::PxTransform transform(mymatrix);

	//if (Parent) {
	physx::PxArticulationLink* link = articulation->createLink(Parent, transform);
	physx::PxBoxGeometry linkGeometry = physx::PxBoxGeometry(0.01f, 0.01f, 0.1f);
	physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.5, 0.5, 0.5);
	physx::PxRigidActorExt::createExclusiveShape(*link, linkGeometry, *material);
	physx::PxRigidBodyExt::updateMassAndInertia(*link, 1.0f);

	physx::PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();

	if (joint) {
		joint->setJointType(physx::PxArticulationJointType::ePRISMATIC);
		// revolute joint that rotates about the z axis (eSWING2) of the joint frames
		joint->setMotion(physx::PxArticulationAxis::eTWIST, physx::PxArticulationMotion::eFREE);
		joint->setParentPose(pos);
		joint->setChildPose(transform);
	}
	if (m_pChild) {

		m_pChild->CreateArticulation(articulation, link, transform);
	}
	if (m_pSibling) {

		m_pSibling->CreateArticulation(articulation, Parent, pos);
	}
	/*}
	else {

	}*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBoundingBoxObject::CBoundingBoxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();

	std::shared_ptr<CBoundingBoxMesh> pMesh = std::make_shared<CBoundingBoxMesh>(pd3dDevice, pd3dCommandList, xmf3AABBCenter, xmf3AABBExtents);
	m_pMesh = pMesh;
}
CBoundingBoxObject::~CBoundingBoxObject()
{
}
void CBoundingBoxObject::PrepareRender()
{
}
void CBoundingBoxObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (m_pMesh)
	{
		UpdateShaderVariables(pd3dCommandList);

		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);
	}
}

void CMapObject::LoadObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char ModelpstrToken[64] = { '\0' };
	int nSkinnedMeshes = 0;

	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, ModelpstrToken))
		{
			if (!strcmp(ModelpstrToken, "<Hierarchy>:"))
			{
				LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, NULL, pInFile, &nSkinnedMeshes);
				::ReadStringFromFile(pInFile, ModelpstrToken); //"</Hierarchy>"
			}
		}
		else
		{
			break;
		}
	}
}

void CMapObject::AddPhysicsScene(const XMFLOAT4X4& xmfWorld)
{
	physx::PxTolerancesScale scale = Locator.GetPxPhysics()->getTolerancesScale();
	physx::PxCookingParams params(scale);

	params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	physx::PxTriangleMeshDesc meshDesc;
	std::vector<XMFLOAT3> vertexs = m_pMesh->GetVertexs();
	std::vector<XMFLOAT3> world_vertexs; world_vertexs.resize(vertexs.size());
	XMMATRIX trans = XMLoadFloat4x4(&m_xmf4x4Transform);

	XMVECTOR scaling;
	XMVECTOR rotation;
	XMVECTOR translation;
	XMMatrixDecompose(&scaling, &rotation, &translation, XMLoadFloat4x4(&m_xmf4x4Transform));
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	XMMATRIX scalingMatrix = XMMatrixScalingFromVector(scaling);
	XMMATRIX transformMatrix = scalingMatrix * rotationMatrix;

	trans.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	int index = 0;
	for (XMFLOAT3 pos : vertexs) {
		XMFLOAT3 point = Vector3::TransformCoord(pos, transformMatrix);
		world_vertexs[index++] = point;
	}
	meshDesc.points.count = vertexs.size();
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = world_vertexs.data();

	std::vector<UINT> Indices = m_pMesh->GetIndices();
	meshDesc.triangles.count = Indices.size() / 3;
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = Indices.data();


	physx::PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc, Locator.GetPxPhysics()->getPhysicsInsertionCallback());

	physx::PxTransform transform = physx::PxTransform(xmfWorld._41, xmfWorld._42, xmfWorld._43);
	transform.q.normalize();
	physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.5, 0.5, 0.5);

	physx::PxRigidStatic* actor = physx::PxCreateStatic(*Locator.GetPxPhysics(), transform, physx::PxTriangleMeshGeometry(aTriangleMesh), *material);
	Locator.GetPxScene()->addActor(*actor);
}

bool CMapObject::CheckCollision(CGameObject* pTargetObject)
{
	BoundingOrientedBox* playerBoundingBox = pTargetObject->GetBoundingBox();

	if (!Vector3::Length(m_ObjectBoundingBox.Extents))
		return false;

	bool bCollision = m_TransformedObjectBoundingBox.Intersects(*playerBoundingBox);

	if (bCollision)
	{
		m_Objtype = MAP_OBJ_TYPE::TREE;
	}

	return bCollision;
}

void CMapObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CGameObject::UpdateTransform(pxmf4x4Parent);

	if (pBoundingBoxMesh)
		pBoundingBoxMesh->SetWorld(m_xmf4x4World);

	m_ObjectBoundingBox.Transform(m_TransformedObjectBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
}

void CMapObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BoundingOrientedBox aambb = CreateAAMBB();
	m_ObjectBoundingBox = BoundingOrientedBox{ aambb.Center, aambb.Extents, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
#ifdef RENDER_BOUNDING_BOX
	pBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, aambb.Center, aambb.Extents);
#endif // RENDER_BOUNDING_BOX
}

BoundingOrientedBox CMapObject::CreateAAMBB()
{
	BoundingOrientedBox bbox;

	switch (m_Objtype)
	{
	case MAP_OBJ_TYPE::ROCK:
		bbox.Center = m_pMesh->GetBoundingCenter();
		bbox.Extents = m_pMesh->GetBoundingExtent();
		break;
	case MAP_OBJ_TYPE::TREE:
	case MAP_OBJ_TYPE::PROP:
	{
		BoundingOrientedBox tmpbox;
		XMFLOAT3 minCoord = minCoord = { FLT_MAX, FLT_MAX, FLT_MAX };
		XMFLOAT3 maxCoord = maxCoord = { FLT_MIN, FLT_MIN, FLT_MIN };

		float centerY = m_pMesh->GetBoundingCenter().y;

		for (auto& pos : m_pMesh->GetVertexs())
		{
			if (pos.y < centerY * 0.7f)
			{
				minCoord.x = min(minCoord.x, pos.x);
				minCoord.y = min(minCoord.y, pos.y);
				minCoord.z = min(minCoord.z, pos.z);

				maxCoord.x = max(maxCoord.x, pos.x);
				maxCoord.y = max(maxCoord.y, pos.y);
				maxCoord.z = max(maxCoord.z, pos.z);
			}
		}

		tmpbox.Extents = XMFLOAT3{ (maxCoord.x - minCoord.x) * 0.5f, (maxCoord.y - minCoord.y) * 0.5f, (maxCoord.z - minCoord.z) * 0.5f };
		tmpbox.Center = XMFLOAT3{ minCoord.x + tmpbox.Extents.x, minCoord.y + tmpbox.Extents.y, minCoord.z + tmpbox.Extents.z };

		bbox.Center = XMFLOAT3{ tmpbox.Center.x, m_pMesh->GetBoundingCenter().y, tmpbox.Center.z };
		bbox.Extents = XMFLOAT3{ tmpbox.Extents.x , m_pMesh->GetBoundingExtent().y, tmpbox.Extents.z };
	}
	break;
	}


	return bbox;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAngrybotObject::CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	CLoadedModelInfo* pAngrybotModel = CModelManager::GetInst()->GetModelInfo("Object/Angrybot.bin");
	if (!pAngrybotModel) pAngrybotModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Angrybot.bin");

	SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pAngrybotModel);
}
CAngrybotObject::~CAngrybotObject()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CEagleObject::CEagleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	CLoadedModelInfo* pEagleModel = CModelManager::GetInst()->GetModelInfo("Object/Eagle.bin");;
	if (!pEagleModel) pEagleModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Eagle.bin");

	SetChild(pEagleModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pEagleModel);
}
CEagleObject::~CEagleObject()
{
}
void CEagleObject::Animate(float fTimeElapsed)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position.z -= 20.0f * fTimeElapsed;
	Rotate(0.0f, 0.0f, 0.0f);
	SetPosition(xmf3Position);
	CGameObject::Animate(fTimeElapsed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CLionObject::CLionObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	CLoadedModelInfo* pLionModel = CModelManager::GetInst()->GetModelInfo("Object/SK_FKnight_WeaponB_01.bin");;
	if (!pLionModel) pLionModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_FKnight_WeaponB_01.bin");

	SetChild(pLionModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pLionModel);
}
CLionObject::~CLionObject()
{
}
void CLionObject::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
}