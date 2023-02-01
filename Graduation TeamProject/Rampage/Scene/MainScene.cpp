#include "MainScene.h"
#include "..\Global\Timer.h"
#include "..\Object\Texture.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\ModelShader.h"
#include "..\Sound\SoundManager.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Shader\DepthRenderShader.h"

void CMainTMPScene::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	CDepthRenderShader::GetInst()->PrepareShadowMap(pd3dCommandList, fTimeElapsed);
	CDepthRenderShader::GetInst()->UpdateDepthTexture(pd3dCommandList);
}
void CMainTMPScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CMainTMPScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];
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

	D3D12_ROOT_PARAMETER pd3dRootParameters[10];

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
void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);

	DXGI_FORMAT pdxgiObjectRtvFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };

	CBoundingBoxShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	CDepthRenderShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	CDepthRenderShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);
	CDepthRenderShader::GetInst()->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	CDepthRenderShader::GetInst()->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, CDepthRenderShader::GetInst()->GetDepthTexture()->GetTextures());
	CDepthRenderShader::GetInst()->CreateShaderResourceViews(pd3dDevice, CDepthRenderShader::GetInst()->GetDepthTexture(), 0, 9);

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	CModelShader::GetInst()->CreateShaderVariables(pd3dDevice,pd3dCommandList);
	CModelShader::GetInst()->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 100);
	
	CSoundManager::GetInst()->RegisterSound("Sound/mp3/David Bowie - Starman.mp3", false);
	//CSoundManager::GetInst()->PlaySound("Sound/mp3/David Bowie - Starman.mp3");

	std::unique_ptr<CKnightObject> m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pObject->SetScale(15.0f, 15.0f, 15.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(-15.0f, 15.0f, 15.0f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_pObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(15.0f, -15.0f, -15.0f));
	m_pObject->SetScale(10.0f, 10.0f, 10.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_pObjects.push_back(std::move(m_pObject));

	// Light 持失
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	//// Terrain Shader 持失
	//m_pTerrainShader = std::make_unique<CTerrainShader>();
	//m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 2, pdxgiObjectRtvFormats, 0);
	//m_pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	//m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//// Terrain 持失
	//XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	//XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	//m_pTerrain = std::make_unique<CHeightMapTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Image/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	//m_pTerrain->SetPosition(XMFLOAT3(-800.f, -750.f, -800.f));
	////m_pTerrain->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 13 + CDepthRenderShader::GetInst()->GetDepthTexture()->GetTextures());
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	

	// Terrain 持失
	XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Terrain/terrainHeightMap257.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	m_pTerrain->SetPosition(XMFLOAT3(-800.f, -600.f, -800.f));

	CDepthRenderShader::GetInst()->SetObjects(&m_pObjects);
	CDepthRenderShader::GetInst()->SetLight(m_pLight->GetLights());
	CDepthRenderShader::GetInst()->SetTerrain(m_pTerrain.get());
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
		m_pObjects[i]->Animate(0.0f);
		m_pObjects[i]->Render(pd3dCommandList, true);
	}

	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);

	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
}


