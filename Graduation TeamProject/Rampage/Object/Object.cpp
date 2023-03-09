#include "Texture.h"
#include "Mesh.h"
#include "Object.h"
#include "ModelManager.h"
#include "..\Shader\Shader.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Global\MessageDispatcher.h"
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
}
void CGameObject::ReleaseShaderVariables()
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh)
		m_pMesh->ReleaseUploadBuffers();
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
void CGameObject::ChangeTexture(std::shared_ptr<CTexture> pTexture)
{
	if (m_ppMaterials.data())
		m_ppMaterials[0]->SetTexture(pTexture);
	else
		SetTexture(pTexture);
}

void CGameObject::Update(float fTimeElapsed)
{
	Animate(fTimeElapsed);
}
void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
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

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
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
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		/*else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}*/
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, CModelShader::GetInst(), 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
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
		//joint->setMotion(physx::PxArticulationAxis::eSWING2, physx::PxArticulationMotion::eLIMITED);
		//physx::PxArticulationLimit limits;
		//limits.low = -physx::PxPiDivFour;  // in rad for a rotational motion
		//limits.high = physx::PxPiDivFour;
		//joint->setLimitParams(physx::PxArticulationAxis::eSWING2, limits);

		//physx::PxArticulationDrive posDrive;
		//posDrive.stiffness = driveStiffness;                      // the spring constant driving the joint to a target position
		//posDrive.damping = driveDamping;                        // the damping coefficient driving the joint to a target velocity
		//posDrive.maxForce = actuatorLimit;                        // force limit for the drive
		//posDrive.driveType = PxArticulationDriveType::eFORCE;

		//joint->setDriveParams(physx::PxArticulationAxis::eSWING2, posDrive);
		//joint->setDriveVelocity(physx::PxArticulationAxis::eSWING2, 0.0f);
		//joint->setDriveTarget(physx::PxArticulationAxis::eSWING2, targetPosition);

		joint->setParentPose(pos);

		/*physx::PxMat44 matrix;
		XMFLOAT4X4 childTransform = m_pChild->m_xmf4x4Transform;
		XMFLOAT3 childpos(m_pChild->GetPosition());
		childTransform._41 = 0;
		childTransform._42 = 0;
		childTransform._43 = 0;
		memcpy(&matrix, &Matrix4x4::Inverse(childTransform), sizeof(physx::PxMat44));

		matrix.column3.x = childpos.x;
		matrix.column3.y = childpos.y;
		matrix.column3.z = childpos.z;

		physx::PxTransform childPx(matrix);*/
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
#define KNIGHT_ROOT_MOTION
CKnightObject::CKnightObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	CLoadedModelInfo* pKnightModel = CModelManager::GetInst()->GetModelInfo("Object/SK_FKnightB_long_sword.bin");;
	if (!pKnightModel) pKnightModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_FKnightB_long_sword.bin");

	CGameObject* obj = pKnightModel->m_pModelRootObject->FindFrame("SK_FKnightB_05");
	//obj->m_ppMaterials[8]->m_pTexture->m_nTextureType = 1;
	//obj->m_ppMaterials[5]->m_nType = 1; // 머리 노멀 맵 사용 x
	//obj->m_ppMaterials[7]->m_nType = 1; // 가슴 밴드 노멀 맵 사용 x
	//obj->m_ppMaterials[8]->m_nType = 1; // 피부 노멀 맵 사용 x

	SetChild(pKnightModel->m_pModelRootObject, true);
	//m_pSkinnedAnimationController = std::make_unique<CKightRootRollBackAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pKnightModel);
	m_pSkinnedAnimationController = std::make_unique<CKightNoMoveRootAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pKnightModel);

	auto Find_Frame_Index = [](std::string& target, std::vector<std::string>& source) {
		int cnt = 0;
		for (std::string& str : source) {
			if (strcmp(str.c_str(), target.c_str()) == 0)
				break;
			cnt++;
		}
		return cnt;
	};



	//CSkinnedMesh* Mesh = (CSkinnedMesh*)obj->m_pMesh.get();

	//static int test = 0;


	//if (test == 0) {
	//	m_pArticulation = Locator.GetPxPhysics()->createArticulationReducedCoordinate();

	//	physx::PxTransform transform(physx::PxVec3(0.0f, 0.0f, 0.0f));
	//	m_pChild->m_pChild->CreateArticulation(m_pArticulation, NULL, transform);

	//	Locator.GetPxScene()->addArticulation(*m_pArticulation);

	//	test++;
	//}


	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
#ifdef KNIGHT_ROOT_MOTION
	m_pSkinnedAnimationController->m_pRootMotionObject = pKnightModel->m_pModelRootObject->FindFrame("root");
	if (m_pSkinnedAnimationController->m_pRootMotionObject) {
		m_pSkinnedAnimationController->m_bRootMotion = true;
		m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(5.0f, 5.0f, 5.0f);
	}
#endif // KNIGHT_ROOT_MOTION
}
CKnightObject::~CKnightObject()
{
	/*if (Rigid) {
		Locator.GetPxScene()->removeActor(*Rigid);
	}*/
}
void CKnightObject::SetRigidDynamic()
{
	physx::PxPhysics* pPhysics = Locator.GetPxPhysics();

	physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));

	physx::PxMaterial* material = pPhysics->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* shape = pPhysics->createShape(physx::PxBoxGeometry(3.0f, 5.0f, 3.0f), *material);

	physx::PxRigidDynamic* actor = physx::PxCreateDynamic(*pPhysics, transform, physx::PxBoxGeometry(3.0f, 5.0f, 3.0f), *material, 1.0f);

	Rigid = actor;

	//Locator.GetPxScene()->addActor(*Rigid);


	//
	//physx::PxArticulationReducedCoordinate* Articulation = pPhysics->createArticulationReducedCoordinate();
	//Articulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, true);

}
bool CKnightObject::CheckCollision(CGameObject* pTargetObject)
{
	bool flag = false;
	if (pTargetObject)
	{
		BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
		if (m_TransformedWeaponBoundingBox.Intersects(TargetBoundingBox)) {

			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("Collide\n"));
			OutputDebugString(pstrDebug);

			flag = true;
		}
	}
	return flag;
}
void CKnightObject::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
	if (Rigid) {
		physx::PxRigidDynamic* actor = (physx::PxRigidDynamic*)Rigid;
		physx::PxTransform transform = actor->getGlobalPose();

		physx::PxMat44 Matrix(transform);
		Matrix = Matrix.inverseRT();
		m_pChild->m_xmf4x4Transform._11 = Matrix.column0.x; m_pChild->m_xmf4x4Transform._12 = Matrix.column0.y; m_pChild->m_xmf4x4Transform._13 = Matrix.column0.z;
		m_pChild->m_xmf4x4Transform._21 = Matrix.column1.x; m_pChild->m_xmf4x4Transform._22 = Matrix.column1.y; m_pChild->m_xmf4x4Transform._23 = Matrix.column1.z;
		m_pChild->m_xmf4x4Transform._31 = Matrix.column2.x; m_pChild->m_xmf4x4Transform._32 = Matrix.column2.y; m_pChild->m_xmf4x4Transform._33 = Matrix.column2.z;
		//m_xmf4x4World._41 = Matrix.column3.x; m_xmf4x4World._42 = Matrix.column3.y; m_xmf4x4World._43 = Matrix.column3.z; m_xmf4x4World._44 = Matrix.column3.w;
		SetPosition(XMFLOAT3(transform.p.x, transform.p.y, transform.p.z));
	}
}
void CKnightObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
	m_BodyBoundingBox.Transform(m_TransformedBodyBoundingBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();
		XMFLOAT3 xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
		XMFLOAT3 xmf3Direction = XMFLOAT3{ xmf4x4World._31, xmf4x4World._32, xmf4x4World._33 };
		xmf3Position = Vector3::Add(xmf3Position, xmf3Direction, -0.8f);
		xmf4x4World._41 = xmf3Position.x;
		xmf4x4World._42 = xmf3Position.y;
		xmf4x4World._43 = xmf3Position.z;
		pWeaponBoundingBoxMesh->SetWorld(xmf4x4World);

		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoundingBox, XMLoadFloat4x4(&xmf4x4World));
	}
}
void CKnightObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("Weapon_r");
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.4f, 0.8f), XMFLOAT3(0.025f, 0.55f, 0.125f));
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.4f, 0.8f), XMFLOAT3(0.025f, 0.55f, 0.125f) };
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//

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
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CKightRootRollBackAnimationController::CKightRootRollBackAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}
CKightRootRollBackAnimationController::~CKightRootRollBackAnimationController()
{
}
#define _WITH_DEBUG_ROOT_MOTION
void CKightRootRollBackAnimationController::OnRootMotion(CGameObject* pRootGameObject)
{
	if (m_bRootMotion)
	{
		if (m_pAnimationTracks[0].m_fPosition == 0.0f)
		{
			m_xmf3FirstRootMotionPosition = m_pRootMotionObject->GetPosition();
		}
		else if (m_pAnimationTracks[0].m_fPosition < 0.0f)
		{
			XMFLOAT3 xmf3Position = m_pRootMotionObject->GetPosition();

			//		XMFLOAT3 xmf3RootPosition = m_pModelRootObject->GetPosition();
			//		pRootGameObject->m_xmf4x4ToParent = m_pModelRootObject->m_xmf4x4World;
			//		pRootGameObject->SetPosition(xmf3RootPosition);
			//XMFLOAT3 xmf3Offset = Vector3::Subtract(xmf3Position, m_xmf3FirstRootMotionPosition);
			XMFLOAT3 xmf3Offset = Vector3::Subtract(m_xmf3FirstRootMotionPosition, xmf3Position);
			xmf3Offset = {
				xmf3Offset.x * m_xmf3RootObjectScale.x,
				xmf3Offset.y * m_xmf3RootObjectScale.y,
				xmf3Offset.z * m_xmf3RootObjectScale.z
			};

			//			xmf3Position = pRootGameObject->GetPosition();
			//			XMFLOAT3 xmf3Look = pRootGameObject->GetLook();
			//			xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fabs(xmf3Offset.x));

			pRootGameObject->m_xmf4x4Transform._41 += xmf3Offset.x;
			pRootGameObject->m_xmf4x4Transform._42 += xmf3Offset.y;
			pRootGameObject->m_xmf4x4Transform._43 += xmf3Offset.z;

			//			pRootGameObject->MoveForward(fabs(xmf3Offset.x));
			//			pRootGameObject->SetPosition(xmf3Position);
			//			m_xmf3PreviousPosition = xmf3Position;
#ifdef _WITH_DEBUG_ROOT_MOTION
			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("Offset: (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n"), xmf3Offset.x, xmf3Offset.y, xmf3Offset.z, pRootGameObject->m_xmf4x4Transform._41, pRootGameObject->m_xmf4x4Transform._42, pRootGameObject->m_xmf4x4Transform._43);
			OutputDebugString(pstrDebug);
#endif
		}
	}
}
CKightNoMoveRootAnimationController::CKightNoMoveRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}
CKightNoMoveRootAnimationController::~CKightNoMoveRootAnimationController()
{
}
void CKightNoMoveRootAnimationController::OnRootMotion(CGameObject* pRootGameObject)
{
	if (m_bRootMotion)
	{
		m_xmf3FirstRootMotionPosition = pRootGameObject->GetPosition();
		m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._42 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
	}
}

