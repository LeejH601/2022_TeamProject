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
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 7;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0 ~ t6: MappingTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 13;
	pd3dDescriptorRanges[1].BaseShaderRegister = 10; //t10~: Mapping Base Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 24; //t24: gtxtTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[9];

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

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

	HRESULT hResult = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	hResult = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

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

	CSoundManager::GetInst()->RegisterSound("Sound/mp3/David Bowie - Starman.mp3", false);
	//CSoundManager::GetInst()->PlaySound("Sound/mp3/David Bowie - Starman.mp3");

	std::unique_ptr<COrcObject> m_pObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(5.0f, 2.0f, -13.4f));
	m_pObject->SetScale(5.0f, 5.0f, 5.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(-15.0f, 0.0f, 0.0f));
	m_pObject->SetScale(2.5f, 2.5f, 2.5f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_pObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pObjects.push_back(std::move(m_pObject));

	m_pObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pObject->SetPosition(XMFLOAT3(15.0f, 0.0f, 0.0f));
	m_pObject->SetScale(3.0f, 3.0f, 3.0f);
	m_pObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
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
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);
	m_pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// Terrain 持失
	XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Terrain/terrainHeightMap257.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	m_pTerrain->SetPosition(XMFLOAT3(-800.f, -750.f, -800.f));

	m_pBillBoardObjectShader = std::make_unique<CBillBoardObjectShader>();
	m_pBillBoardObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, &pdxgiObjectRtvFormats, 0);
	m_pBillBoardObjectShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 10);
	
	std::unique_ptr<CBillBoardObject> pBillBoardObject = std::make_unique<CBillBoardObject>(m_pBillBoardObjectShader->Load_Texture(pd3dDevice, pd3dCommandList, L"Image/Grass01.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get());
	pBillBoardObject->SetPosition(XMFLOAT3(-10.f, 0.0f, 0.f));
	m_pBillBoardObjects.push_back(std::move(pBillBoardObject));

	pBillBoardObject = std::make_unique<CBillBoardObject>(m_pBillBoardObjectShader->Load_Texture(pd3dDevice, pd3dCommandList, L"Image/Grass02.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get());
	pBillBoardObject->SetPosition(XMFLOAT3(0.f, 0.0f, 15.0f));
	m_pBillBoardObjects.push_back(std::move(pBillBoardObject));

	pBillBoardObject = std::make_unique<CBillBoardObject>(m_pBillBoardObjectShader->Load_Texture(pd3dDevice, pd3dCommandList, L"Image/flower02.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get());
	pBillBoardObject->SetPosition(XMFLOAT3(25.0f, 0.0f, 25.0f));
	m_pBillBoardObjects.push_back(std::move(pBillBoardObject));

	pBillBoardObject = std::make_unique<CBillBoardObject>(m_pBillBoardObjectShader->Load_Texture(pd3dDevice, pd3dCommandList, L"Image/flower02.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get());
	pBillBoardObject->SetPosition(XMFLOAT3(35.0f, 0.0f, 0.f));
	m_pBillBoardObjects.push_back(std::move(pBillBoardObject));
}
void CMainTMPScene::AnimateObjects(float fTimeElapsed)
{

}
void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CCamera* pCamera)
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

	
	for (int i = 0; i < m_pBillBoardObjects.size(); i++)
	{
		m_pBillBoardObjectShader->Render(pd3dCommandList, 0);
		m_pBillBoardObjects[i]->Render(pd3dCommandList, pCamera);
	}

	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
}


