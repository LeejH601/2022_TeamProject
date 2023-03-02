#include "MainScene.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Object\Texture.h"
#include "..\Object\Player.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\ModelShader.h"
//#include "..\Sound\SoundManager.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Shader\ParticleShader.h"
#include "..\Shader\DepthRenderShader.h"
#include "..\Object\Monster.h"
#include "..\Object\TextureManager.h"
#include "..\Global\Locator.h"

void CMainTMPScene::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
	((CPlayer*)m_pPlayer)->SetPlayerUpdatedContext(m_pTerrain.get());

	if (m_pDepthRenderShader.get())
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(pPlayer);
}

void CMainTMPScene::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (m_pDepthRenderShader)
	{
		((CDepthRenderShader*)m_pDepthRenderShader.get())->PrepareShadowMap(pd3dCommandList, 0.0f);
		((CDepthRenderShader*)m_pDepthRenderShader.get())->UpdateDepthTexture(pd3dCommandList);
	}
}
void CMainTMPScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CMainTMPScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 7;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0 ~ t6: MappingTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 13;
	pd3dDescriptorRanges[1].BaseShaderRegister = 10; //t10 ~ t22: Mapping Base Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = MAX_DEPTH_TEXTURES;
	pd3dDescriptorRanges[2].BaseShaderRegister = 23; //t23: DepthWrite Texture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 30; //t30: gtxtTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 3;
	pd3dDescriptorRanges[4].BaseShaderRegister = 31; //t31~33: gtxtParticleTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[13];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 33;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //GameObject
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; // b1 : Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 2; //Skinned Bone Offsets
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 3; //Skinned Bone Transforms
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 5; // b5 : Parallex constant
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[8].Descriptor.ShaderRegister = 6; // b6 : ToLight
	pd3dRootParameters[8].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //Depth Buffer
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; // cbFrameworkInfo
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[12].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[12].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[4];

	d3dSamplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].MipLODBias = 0;
	d3dSamplerDesc[0].MaxAnisotropy = 1;
	d3dSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[0].MinLOD = 0;
	d3dSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[0].ShaderRegister = 0;
	d3dSamplerDesc[0].RegisterSpace = 0;
	d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].MipLODBias = 0;
	d3dSamplerDesc[1].MaxAnisotropy = 1;
	d3dSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	d3dSamplerDesc[1].MinLOD = 0;
	d3dSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister = 1;
	d3dSamplerDesc[1].RegisterSpace = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	d3dSamplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].MipLODBias = 0.0f;
	d3dSamplerDesc[2].MaxAnisotropy = 1;
	d3dSamplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //D3D12_COMPARISON_FUNC_LESS
	d3dSamplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[2].MinLOD = 0;
	d3dSamplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[2].ShaderRegister = 2;
	d3dSamplerDesc[2].RegisterSpace = 0;
	d3dSamplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[3].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].MipLODBias = 0.0f;
	d3dSamplerDesc[3].MaxAnisotropy = 1;
	d3dSamplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[3].MinLOD = 0;
	d3dSamplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[3].ShaderRegister = 3;
	d3dSamplerDesc[3].RegisterSpace = 0;
	d3dSamplerDesc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(d3dSamplerDesc);;
	d3dRootSignatureDesc.pStaticSamplers = d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;

	HRESULT hResult = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	hResult = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
bool CMainTMPScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		// 좌클릭시 사용자가 좌클릭 했음을 표현하는 변수를 true로 바꿔줌
		if (m_pPlayer)
			((CPlayer*)m_pPlayer)->m_bAttack = true;
		break;
	default:
		break;
	}

	return 0;
}
bool CMainTMPScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'f':
		case 'F':
			((CPlayer*)m_pPlayer)->Tmp();
			break;
		case 'w':
		case 'W':
			if (wParam == 'w' || wParam == 'W') dwDirection |= DIR_FORWARD;
			break;
		case 's':
		case 'S':
			if (wParam == 's' || wParam == 'S') dwDirection |= DIR_BACKWARD;
			break;
		case 'a':
		case 'A':
			if (wParam == 'a' || wParam == 'A') dwDirection |= DIR_LEFT;
			break;
		case 'd':
		case 'D':
			if (wParam == 'd' || wParam == 'D') dwDirection |= DIR_RIGHT;
			break;
		case 'q':
		case 'Q':
			if (wParam == 'q' || wParam == 'Q')dwDirection |= DIR_DOWN;
			break;
		case 'e':
		case 'E':
			if (wParam == 'e' || wParam == 'E') dwDirection |= DIR_UP;
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case 'w':
		case 'W':
			if (wParam == 'w' || wParam == 'W') dwDirection &= (~DIR_FORWARD);
			break;
		case 's':
		case 'S':
			if (wParam == 's' || wParam == 'S') dwDirection &= (~DIR_BACKWARD);
			break;
		case 'a':
		case 'A':
			if (wParam == 'a' || wParam == 'A') dwDirection &= (~DIR_LEFT);
			break;
		case 'd':
		case 'D':
			if (wParam == 'd' || wParam == 'D') dwDirection &= (~DIR_RIGHT);
			break;
		case 'q':
		case 'Q':
			if (wParam == 'q' || wParam == 'Q')dwDirection &= (~DIR_DOWN);
			break;
		case 'e':
		case 'E':
			if (wParam == 'e' || wParam == 'E') dwDirection &= (~DIR_UP);
			break;
		}
	default:
		break;
	}

	return 0;
}
void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);

	DXGI_FORMAT pdxgiObjectRtvFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };

	CBoundingBoxShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthRenderShader = std::make_unique<CDepthRenderShader>();
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_pDepthRenderShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture()->GetTextures());
	m_pDepthRenderShader->CreateShaderResourceViews(pd3dDevice, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture(), 0, 9);

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	CModelShader::GetInst()->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CModelShader::GetInst()->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 100);

	//CSoundManager::GetInst()->RegisterSound("Sound/mp3/David Bowie - Starman.mp3", false);
	//CSoundManager::GetInst()->PlaySound("Sound/mp3/David Bowie - Starman.mp3");

	//std::unique_ptr<CKnightObject> m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);

	std::unique_ptr<CKnightObject> m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(-15.0f, 350.0f, 15.0f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_pObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(15.0f, 350.0f, -15.0f));
	m_pObject->SetScale(10.0f, 10.0f, 10.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);

	//int nAnimationSets = m_pObject->m_pSkinnedAnimationController->m_pAnimationSets->m_nAnimationSets;

	//m_pObjects.push_back(std::move(m_pObject));

	std::unique_ptr<COrcObject> m_pOrc = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pOrc->SetPosition(XMFLOAT3(120.0f, 600.0f, 90.0f));
	m_pOrc->SetScale(14.0f, 14.0f, 14.0f);
	m_pOrc->Rotate(0.0f, 0.0f, 0.0f);
	m_pOrc->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_pOrc->CreateArticulation(1.0f);
	m_pOrc->m_bSimulateArticulate = false;
	m_pOrc->Animate(0.0f);
	m_pOrc->m_bSimulateArticulate = true;
	physx::PxMat44 mat = m_pOrc->m_pArticulation->getRootGlobalPose();
	XMFLOAT4X4 rootWorld = m_pOrc->FindFrame("root")->m_xmf4x4World;
	XMFLOAT4X4 gobworld = m_pOrc->FindFrame("pelvis")->m_xmf4x4World;
	/*memcpy(&mat, &gobworld, sizeof(physx::PxMat44));
	mat.column3.x = gobworld._41;
	mat.column3.y = gobworld._42;
	mat.column3.z = gobworld._43;*/
	m_pOrc->m_pArticulation->copyInternalStateToCache(*m_pOrc->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	physx::PxTransform* rootLInkTrans = &m_pOrc->m_pArticulationCache->rootLinkData->transform;
	physx::PxMat44 tobonetrans = physx::PxMat44(*rootLInkTrans);
	tobonetrans.column3.x += rootWorld._41;
	tobonetrans.column3.y += rootWorld._42;
	tobonetrans.column3.z += rootWorld._43;
	*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
	m_pOrc->m_pArticulation->applyCache(*m_pOrc->m_pArticulationCache, physx::PxArticulationCacheFlag::eROOT_TRANSFORM);

	m_pObjects.push_back(std::move(m_pOrc));

	/*std::unique_ptr<CSkeletonObject> m_pSkeleton = std::make_unique<CSkeletonObject>(pd3dDevice, pd3dCommandList, 1);
	m_pSkeleton->SetPosition(XMFLOAT3(150.0f, 0.0f, 90.0f));
	m_pSkeleton->SetScale(14.0f, 14.0f, 14.0f);
	m_pSkeleton->Rotate(0.0f, 0.0f, 0.0f);
	m_pSkeleton->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_pSkeleton->CreateArticulation(0.5f);
	m_pSkeleton->m_bSimulateArticulate = false;
	m_pSkeleton->Animate(0.0f);
	m_pSkeleton->m_bSimulateArticulate = true;
	mat = m_pSkeleton->m_pArticulation->getRootGlobalPose();
	gobworld = m_pSkeleton->FindFrame("pelvis")->m_xmf4x4World;
	mat.column3.x = gobworld._41;
	mat.column3.y = gobworld._42;
	mat.column3.z = gobworld._43;
	m_pSkeleton->m_pArticulation->setRootGlobalPose(physx::PxTransform(mat));
	m_pObjects.push_back(std::move(m_pSkeleton));*/

	std::unique_ptr<CGoblinObject> m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblin->SetPosition(XMFLOAT3(90.0f, 600.0f, 90.0f));
	m_pGoblin->SetScale(14.0f, 14.0f, 14.0f);
	m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
	m_pGoblin->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_pGoblin->CreateArticulation(0.5f);
	m_pGoblin->m_bSimulateArticulate = false;
	m_pGoblin->Animate(0.0f);
	m_pGoblin->m_bSimulateArticulate = true;
	rootWorld = m_pGoblin->FindFrame("root")->m_xmf4x4World;
	m_pGoblin->m_pArticulation->copyInternalStateToCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	rootLInkTrans = &m_pGoblin->m_pArticulationCache->rootLinkData->transform;
	tobonetrans = physx::PxMat44(*rootLInkTrans);
	tobonetrans.column3.x += rootWorld._41;
	tobonetrans.column3.y += rootWorld._42;
	tobonetrans.column3.z += rootWorld._43;
	*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
	m_pGoblin->m_pArticulation->applyCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);

	m_pObjects.push_back(std::move(m_pGoblin));

	m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblin->SetPosition(XMFLOAT3(30.0f, 600.0f, 90.0f));
	m_pGoblin->SetScale(14.0f, 14.0f, 14.0f);
	m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
	m_pGoblin->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_pGoblin->CreateArticulation(0.5f);
	m_pGoblin->m_bSimulateArticulate = false;
	m_pGoblin->Animate(0.0f);
	m_pGoblin->m_bSimulateArticulate = true;
	rootWorld = m_pGoblin->FindFrame("root")->m_xmf4x4World;
	m_pGoblin->m_pArticulation->copyInternalStateToCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	rootLInkTrans = &m_pGoblin->m_pArticulationCache->rootLinkData->transform;
	tobonetrans = physx::PxMat44(*rootLInkTrans);
	tobonetrans.column3.x += rootWorld._41;
	tobonetrans.column3.y += rootWorld._42;
	tobonetrans.column3.z += rootWorld._43;
	*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
	m_pGoblin->m_pArticulation->applyCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	m_pObjects.push_back(std::move(m_pGoblin));

	m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblin->SetPosition(XMFLOAT3(30.0f, 600.0f, 30.0f));
	m_pGoblin->SetScale(14.0f, 14.0f, 14.0f);
	m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
	m_pGoblin->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_pGoblin->CreateArticulation(0.5f);
	m_pGoblin->m_bSimulateArticulate = false;
	m_pGoblin->Animate(0.0f);
	m_pGoblin->m_bSimulateArticulate = true;
	rootWorld = m_pGoblin->FindFrame("root")->m_xmf4x4World;
	m_pGoblin->m_pArticulation->copyInternalStateToCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	rootLInkTrans = &m_pGoblin->m_pArticulationCache->rootLinkData->transform;
	tobonetrans = physx::PxMat44(*rootLInkTrans);
	tobonetrans.column3.x += rootWorld._41;
	tobonetrans.column3.y += rootWorld._42;
	tobonetrans.column3.z += rootWorld._43;
	*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
	m_pGoblin->m_pArticulation->applyCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	m_pObjects.push_back(std::move(m_pGoblin));

	XMFLOAT3 testpos = XMFLOAT3(300.0f, 600.0f, 0.0f);
	for (int i = 0; i < 45; ++i) {
		testpos.x -= 25.0f;
		m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
		m_pGoblin->SetPosition(testpos);
		m_pGoblin->SetScale(14.0f, 14.0f, 14.0f);
		m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
		m_pGoblin->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
		m_pGoblin->CreateArticulation(0.5f);
		m_pGoblin->m_bSimulateArticulate = false;
		m_pGoblin->Animate(0.0f);
		m_pGoblin->m_bSimulateArticulate = true;
		rootWorld = m_pGoblin->FindFrame("root")->m_xmf4x4World;
		m_pGoblin->m_pArticulation->copyInternalStateToCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		rootLInkTrans = &m_pGoblin->m_pArticulationCache->rootLinkData->transform;
		tobonetrans = physx::PxMat44(*rootLInkTrans);
		tobonetrans.column3.x += rootWorld._41;
		tobonetrans.column3.y += rootWorld._42;
		tobonetrans.column3.z += rootWorld._43;
		*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
		physx::PxVec3 randDir = physx::PxVec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
		randDir.normalize();
		randDir *= 10.0f;
		memcpy(&m_pGoblin->m_pArticulationCache->jointForce[3], &randDir, sizeof(physx::PxVec3));
		m_pGoblin->m_pArticulation->applyCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		
		m_pObjects.push_back(std::move(m_pGoblin));
	}

	/*for (int i = 0; i < nAnimationSets; ++i)
	{
		std::unique_ptr<CGameObject> pCharater = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
		pCharater->SetPosition(XMFLOAT3(5.0f * i, 0.0f, 0.0f));
		pCharater->SetScale(5.0f, 5.0f, 5.0f);
		pCharater->Rotate(0.0f, -90.0f, 0.0f);
		pCharater->m_pSkinnedAnimationController->SetTrackAnimationSet(0, i);
		m_pObjects.push_back(std::move(pCharater));
	}*/

	// Light 생성
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);


	// Terrain 생성
	XMFLOAT3 xmf3Scale(6.0f, 3.0f, 6.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Terrain/terrainHeightMap257.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	m_pTerrain->SetPosition(XMFLOAT3(-800.f, 0.f, -800.f));
	m_pTerrain->SetRigidStatic();
	for (int i = 0; i < m_pObjects.size(); ++i)
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pObjects[i].get());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetLight(m_pLight->GetLights());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetTerrain(m_pTerrain.get());

	m_pBillBoardObjectShader = std::make_unique<CBillBoardObjectShader>();
	m_pBillBoardObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);
	m_pBillBoardObjectShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 10);

	
	//std::unique_ptr<CBillBoardObject> pBillBoardObject = std::make_unique<CBillBoardObject>(CTextureManager::GetInst()->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Grass01.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 4.f);
	//pBillBoardObject->SetPosition(XMFLOAT3(0.f, -5.f, 0.f));
	//m_pBillBoardObjects.push_back(std::move(pBillBoardObject));

	//std::unique_ptr<CMultiSpriteObject> pSpriteAnimationObject = std::make_unique<CMultiSpriteObject>(CTextureManager::GetInst()->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 8, 8, 4.f, 0.05f);
	//pSpriteAnimationObject->SetPosition(XMFLOAT3(0.f, 5.f, 0.f));
	//m_pBillBoardObjects.push_back(std::move(pSpriteAnimationObject));

	//pSpriteAnimationObject = std::make_unique<CMultiSpriteObject>(CTextureManager::GetInst()->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 5, 6, 4.f, 0.05f);
	//pSpriteAnimationObject->SetPosition(XMFLOAT3(-10.f, 15.f, 0.f));
	//m_pBillBoardObjects.push_back(std::move(pSpriteAnimationObject));

	//m_pParticleShader = std::make_shared<CParticleShader>();
	//std::unique_ptr<CParticleObject> pParticleObject = std::make_unique<CParticleObject>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), true);
	//
	//m_ppParticleObjects.push_back(std::move(pParticleObject));
}
void CMainTMPScene::UpdateObjects(float fTimeElapsed)
{
	m_pPlayer->Update(fTimeElapsed);

	for (int i = 0; i < m_pObjects.size(); ++i)
		m_pObjects[i]->Update(fTimeElapsed);
}
void CMainTMPScene::AnimateObjects(float fTimeElapsed)
{
	static float SimulateElapsedTime = 0.0f;

	SimulateElapsedTime += fTimeElapsed;
	if (!b_simulation) {
		Locator.GetPxScene()->simulate(SimulateElapsedTime);
		SimulateElapsedTime = 0.0f;
		b_simulation = true;
	}
	if (Locator.GetPxScene()->fetchResults(false)) {
		UpdateObjectArticulation();
		b_simulation = false;
	}

	UpdateObjects(fTimeElapsed);
}
void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	if(pCamera) pCamera->OnPrepareRender(pd3dCommandList);

	m_pLight->Render(pd3dCommandList);

	CModelShader::GetInst()->Render(pd3dCommandList, 0);

	if (m_pPlayer)
	{
		m_pPlayer->Animate(0.0f);
		m_pPlayer->Render(pd3dCommandList, true);
	}

	for (int i = 0; i < m_pObjects.size(); ++i)
	{
		m_pObjects[i]->Animate(0.0f);
		m_pObjects[i]->Render(pd3dCommandList, true);
	}

	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);

#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
#endif
	/*m_pBillBoardObjectShader->Render(pd3dCommandList, 0);
	for (int i = 0; i < m_pBillBoardObjects.size(); i++)
	{
		m_pBillBoardObjects[i]->Animate(fTimeElapsed);
		m_pBillBoardObjects[i]->Render(pd3dCommandList, true);
	}
	*/

	//for (int i = 0; i < m_ppParticleObjects.size(); i++)
	//{
	//	//m_pParticleShader->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//	m_ppParticleObjects[i]->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//	m_ppParticleObjects[i]->Render(pd3dCommandList, pCamera, m_pParticleShader.get());
	//}
}

void CMainTMPScene::OnPostRenderTarget()
{
	//for (int i = 0; i < m_ppParticleObjects.size(); i++)
	//{
	//	m_ppParticleObjects[i]->OnPostRender();
	//}

}


