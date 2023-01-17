#include "MainScene.h"
#include "..\Global\Timer.h"
#include "..\Object\Texture.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\ModelShader.h"
#include "..\Sound\SoundManager.h"
#include "..\Shader\BoundingBoxShader.h"

void CMainTMPScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CMainTMPScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges;
	pd3dDescriptorRanges.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges.NumDescriptors = 7;
	pd3dDescriptorRanges.BaseShaderRegister = 0; //t0 ~ t6: MappingTexture
	pd3dDescriptorRanges.RegisterSpace = 0;
	pd3dDescriptorRanges.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

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
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges;
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

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;

	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);

	DXGI_FORMAT pdxgiObjectRtvFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };

	CBoundingBoxShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);
	CModelShader::GetInst()->CreateShaderVariables(pd3dDevice,pd3dCommandList);
	CModelShader::GetInst()->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 100);

	CModelManager::GetInst()->LoadModel(pd3dDevice,pd3dCommandList, "Object/Angrybot.bin");;
	CModelManager::GetInst()->LoadModel(pd3dDevice,pd3dCommandList, "Object/Eagle.bin");
	CModelManager::GetInst()->LoadModel(pd3dDevice,pd3dCommandList, "Object/Lion.bin");
	CModelManager::GetInst()->LoadModel(pd3dDevice,pd3dCommandList, "Object/SK_FKnight_WeaponB_01.bin");

	CSoundManager::GetInst()->RegisterSound("Sound/mp3/David Bowie - Starman.mp3", false);
	//CSoundManager::GetInst()->PlaySound("Sound/mp3/David Bowie - Starman.mp3");

	std::unique_ptr<CGoblinObject> m_pObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(-15.0f, 0.0f, 0.0f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(15.0f, 0.0f, 0.0f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_pObjects.push_back(std::move(m_pObject));

	// Light 持失
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	// Terrain Shader 持失
	m_pTerrainShader = std::make_unique<CTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);
	m_pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// Terrain 持失
	XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CHeightMapTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Image/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	m_pTerrain->SetPosition(XMFLOAT3(-800.f, -750.f, -800.f));
	//m_pTerrain->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
}
void CMainTMPScene::AnimateObjects(float fTimeElapsed)
{
	
}
void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	m_pLight->Render(pd3dCommandList);

	CModelShader::GetInst()->Render(pd3dCommandList, 0);

	for (int i = 0; i < m_pObjects.size(); ++i)
	{
		m_pObjects[i]->Animate(fTimeElapsed * (i + 1));
		if (!m_pObjects[i]->m_pSkinnedAnimationController) m_pObjects[i]->UpdateTransform(NULL);
		m_pObjects[i]->Render(pd3dCommandList);
	}

	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList);

	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
}


