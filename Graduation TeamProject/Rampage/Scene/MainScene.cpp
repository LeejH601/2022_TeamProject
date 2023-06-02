#include "MainScene.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\Texture.h"
#include "..\Object\Player.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Shader\ParticleShader.h"
#include "..\Shader\DepthRenderShader.h"
#include "..\Object\Monster.h"
#include "..\Object\TextureManager.h"
#include "..\Object\PlayerParticleObject.h"

#include <PxForceMode.h>

void CMainTMPScene::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
	((CPlayer*)m_pPlayer)->SetUpdatedContext(m_pMap.get());
	((CPlayer*)m_pPlayer)->SetCamera(m_pMainSceneCamera.get());
	((CThirdPersonCamera*)m_pMainSceneCamera.get())->SetPlayer((CPlayer*)m_pPlayer);

	if (m_pDepthRenderShader.get())
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(pPlayer);

	m_pCollisionChecker->RegisterObject(pPlayer);
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
	pd3dDescriptorRanges[0].NumDescriptors = 8;
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
	pd3dDescriptorRanges[3].NumDescriptors = 30;
	pd3dDescriptorRanges[3].BaseShaderRegister = 50; //t50: gtxtTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 7;
	pd3dDescriptorRanges[4].BaseShaderRegister = 35; //t35: gtxMultiRenderTargetTextures
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[15];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 34;
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
	pd3dRootParameters[7].Descriptor.ShaderRegister = 5; // b5 : Dissolve
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

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 9;
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[14].Descriptor.ShaderRegister = 8; // b8 : cbLensFlarePOsitions
	pd3dRootParameters[14].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[5];

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

	d3dSamplerDesc[4].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].MipLODBias = 0;
	d3dSamplerDesc[4].MaxAnisotropy = 1;
	d3dSamplerDesc[4].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[4].MinLOD = 0;
	d3dSamplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[4].ShaderRegister = 4;
	d3dSamplerDesc[4].RegisterSpace = 0;
	d3dSamplerDesc[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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
void CMainTMPScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 50; //t50: Texture2D
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //u0: RWTexture2D
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[2].NumDescriptors = 5;
	pd3dDescriptorRanges[2].BaseShaderRegister = 5; //u5 ~ u9: RWTexture2D
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[3].NumDescriptors = 5;
	pd3dDescriptorRanges[3].BaseShaderRegister = 10; //u10 ~ u14: RWTexture2D
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 5;
	pd3dDescriptorRanges[4].BaseShaderRegister = 55; //t55 ~ 59: Texture2D
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[0].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Texture2D
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //RWTexture2D
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; // BloomFilterTextures
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3]; // BluredTextures
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4]; // BluredTextures
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[5].Constants.Num32BitValues = 4;
	pd3dRootParameters[5].Constants.ShaderRegister = 0; //Bloom Level
	pd3dRootParameters[5].Constants.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;

	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
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
	hResult = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dComputeRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
void CMainTMPScene::UpdateObjectArticulation()
{
	for (int i = 0; i < m_pEnemys.size(); ++i) {
		m_pEnemys[i]->updateArticulationMatrix();
	}

	m_pMap->UpdateObjectArticulation();
}
void CMainTMPScene::RequestRegisterArticulation(RegisterArticulationParams param)
{
	m_lRequestObjects.emplace_back(param);
}
void CMainTMPScene::RegisterArticulations()
{
	for (RegisterArticulationParams& param : m_lRequestObjects) {
		CGameObject* obj = param.pObject;
		bool flag = false;
		flag = Locator.GetPxScene()->addArticulation(*obj->m_pArticulation);

		int index = 0;
		for (XMFLOAT4X4& world : obj->m_AritculatCacheMatrixs) {
			physx::PxMat44 mat = obj->m_pArticulationLinks[index++]->getGlobalPose();

			memcpy(&world, &mat, sizeof(XMFLOAT4X4));
			XMFLOAT3 scale = ((CPhysicsObject*)obj)->GetScale();
			world = Matrix4x4::Multiply(XMMatrixScaling(scale.x, scale.y, scale.z), world);
		}

		if (flag) {
			obj->m_pArticulationCache = obj->m_pArticulation->createCache();
			obj->m_nArtiCache = obj->m_pArticulation->getCacheDataSize();
		}
		else {

		}

		obj->m_bSimulateArticulate = false;
		obj->Animate(0.0f);
		((CPhysicsObject*)obj)->UpdateTransform(NULL);
		XMFLOAT4X4 rootWorld = obj->FindFrame("root")->GetWorld();
		float _rootWorld[16] = {
			rootWorld._11,rootWorld._12,rootWorld._13,rootWorld._14,
			rootWorld._21,rootWorld._22,rootWorld._23,rootWorld._24,
			rootWorld._31,rootWorld._32,rootWorld._33,rootWorld._34,
			rootWorld._41,rootWorld._42,rootWorld._43,rootWorld._44,
		}; 
		obj->m_pArticulation->copyInternalStateToCache(*obj->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		physx::PxTransform* rootLInkTrans = &obj->m_pArticulationCache->rootLinkData->transform;
		physx::PxMat44 tobonetrans = physx::PxMat44(_rootWorld);

		*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
		obj->m_pArticulation->applyCache(*obj->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		obj->m_bSimulateArticulate = true;

		XMFLOAT3 force = param.m_force;
		obj->m_pArticulationLinks[1]->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eIMPULSE);
	}
	m_lRequestObjects.clear();
}
bool CMainTMPScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		// 좌클릭시 사용자가 좌클릭 했음을 표현하는 변수를 true로 바꿔줌
		if (m_pPlayer)
		{
			if (!((CPlayer*)m_pPlayer)->m_bAttack)
			{
				((CPlayer*)m_pPlayer)->m_bAttack = true;
			}
		}
		break;
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

	return 0;
}
SCENE_RETURN_TYPE CMainTMPScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	static int CursorHideCount = 0;

	MOUSE_CUROSR_MODE eMouseMode = Locator.GetMouseCursorMode();

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SHIFT:
			if (m_pPlayer)
			{
				if (!((CPlayer*)m_pPlayer)->m_bEvasioned)
				{
					((CPlayer*)m_pPlayer)->m_bEvasioned = true;
				}
			}
			break;
		case VK_F5:
		{
			XMFLOAT3 xmf3MonsterPos[5];
			XMFLOAT3 xmf3PlayerPos = m_pPlayer->GetPosition();
			XMFLOAT3 xmf3PlayerLook = m_pPlayer->GetLook();
			XMFLOAT3 xmf3PlayerRight = m_pPlayer->GetRight();
			xmf3PlayerLook = Vector3::ScalarProduct(xmf3PlayerLook, 20.f, false);

			for (int i = 0; i < 5; i++)
				xmf3MonsterPos[i] = Vector3::Add(Vector3::Add(Vector3::ScalarProduct(xmf3PlayerRight, -20 + i * 10, false), xmf3PlayerLook), xmf3PlayerPos);

			CMonsterPool::GetInst()->SpawnMonster(7, xmf3MonsterPos);
			break;
		}
		case VK_BACK:
			if (m_CurrentMouseCursorMode == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE)
			{
				m_pCurrentCamera = m_pFloatingCamera.get();
				Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
				m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;
				PostMessage(hWnd, WM_ACTIVATE, 0, 0);

				while (CursorHideCount > 0)
				{
					CursorHideCount--;
					ShowCursor(true);
				}
				ClipCursor(NULL);
			}

			return SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE;
		case '1':
		{
			RECT screenRect;
			GetWindowRect(hWnd, &screenRect);
			m_ScreendRect = screenRect;

			m_pCurrentCamera = m_pFloatingCamera.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);

			while (CursorHideCount > 0)
			{
				CursorHideCount--;
				ShowCursor(true);
			}
			ClipCursor(NULL);
		}
		break;
		case '2':
		{

			RECT screenRect;
			GetWindowRect(hWnd, &screenRect);
			m_ScreendRect = screenRect;

			m_pCurrentCamera = m_pMainSceneCamera.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::THIRD_FERSON_MODE);
			m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::THIRD_FERSON_MODE;
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);

			if (CursorHideCount < 1) {
				CursorHideCount++;
				ShowCursor(false);
				ClipCursor(&screenRect);
			}
		}
		break;
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

	return SCENE_RETURN_TYPE::NONE;
}
void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);
	CreateComputeRootSignature(pd3dDevice);

	std::unique_ptr<RegisterArticulationListener> listener = std::make_unique<RegisterArticulationListener>();
	listener->SetScene(this);
	listener->SetEnable(true);
	m_pListeners.push_back(std::move(listener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::REQUEST_REGISTERARTI, m_pListeners.back().get(), nullptr);

	UINT ncbElementBytes = ((sizeof(DissolveParams) + 255) & ~255); //256의 배수
	m_pd3dcbDisolveParams = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbDisolveParams->Map(0, NULL, (void**)&m_pcbMappedDisolveParams);

	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	m_pFloatingCamera = std::make_unique<CFloatingCamera>();
	m_pFloatingCamera->Init(pd3dDevice, pd3dCommandList);
	m_pFloatingCamera->SetPosition(XMFLOAT3(0.0f, 400.0f, -100.0f));

	m_pMainSceneCamera = std::make_unique<CThirdPersonCamera>();
	m_pMainSceneCamera->Init(pd3dDevice, pd3dCommandList);

	m_pCurrentCamera = m_pFloatingCamera.get();

#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
#endif

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthRenderShader = std::make_unique<CDepthRenderShader>();
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_pDepthRenderShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture()->GetTextures());
	m_pDepthRenderShader->CreateShaderResourceViews(pd3dDevice, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture(), 0, 9);

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	CModelShader::GetInst()->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CModelShader::GetInst()->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 200);

	std::unique_ptr<CMonster> m_pMonsterObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 1000.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	m_pMonsterObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 100.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	m_pMonsterObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 100.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	m_pMonsterObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 100.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	m_pMonsterObject = std::make_unique<CSkeletonObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 100.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	m_pMonsterObject = std::make_unique<CSkeletonObject>(pd3dDevice, pd3dCommandList, 1);
	m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, -70));
	m_pMonsterObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pMonsterObject->m_fHP = 100.f;
	m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pMonsterObject->CreateArticulation(1.0f);
	m_pEnemys.push_back(std::move(m_pMonsterObject));

	// CollisionChecker 생성
	m_pCollisionChecker = std::make_unique<CollisionChecker>();

	// Light 생성
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	
	m_pMap = std::make_unique<CMap>();
	m_pMap->Init(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	m_pMap->GetTerrain()->SetPosition(XMFLOAT3(86.4804, -46.8876 - 46.8876 * 0.38819 + 6.5f, -183.7856));
	m_pMap->GetTerrain()->SetRigidStatic();
	m_pMap->LoadSceneFromFile(pd3dDevice, pd3dCommandList, "Object/Scene/Scene.bin");

	int index = 0;

	m_IObjectIndexs.resize(m_pEnemys.size() + m_pMap->GetMapObjects().size());

	for (int i = 0; i < m_pEnemys.size(); ++i) {
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pEnemys[i].get());
		((CPhysicsObject*)m_pEnemys[i].get())->SetUpdatedContext(m_pMap.get());
		m_IObjectIndexs[i] = index++;

		m_pCollisionChecker->RegisterObject(m_pEnemys[i].get());
	}

	for (int i = 0; i < m_pMap->GetMapObjects().size(); ++i) {
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pMap->GetMapObjects()[i].get());
		m_IObjectIndexs[i] = index++;
	}

	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetLight(m_pLight->GetLights());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetTerrain(m_pMap->GetTerrain().get());

	m_pSunLightShader = std::make_unique<CSunLightShader>();
	m_pSunLightShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	m_pPostProcessShader = std::make_unique<CPostProcessShader>();
	m_pPostProcessShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pPostProcessShader->BuildObjects(pd3dDevice, pd3dCommandList);

	std::shared_ptr<CTexture> pSkyBoxTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/Epic_BlueSunset.dds", RESOURCE_TEXTURE_CUBE, 0);

	m_pSkyBoxShader = std::make_unique<CSkyBoxShader>();
	m_pSkyBoxShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSkyBoxShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1);
	m_pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture.get(), 0, 10);

	m_pSkyBoxObject = std::make_unique<CSkyBox>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), pSkyBoxTexture);

	m_pTextureManager = std::make_unique<CTextureManager>();
	m_pTextureManager->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 100);
	m_pTextureManager->LoadSphereBuffer(pd3dDevice, pd3dCommandList);
	m_pTextureManager->LoadTexture(TextureType::SmokeTexture, pd3dDevice, pd3dCommandList, L"Image/SmokeImages/Smoke2.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Explode_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Fire_Effect.dds", 5, 6);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Circle1.dds", 1, 1);

	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/TextureFlash2.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Meteor.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Effect0.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_11.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_21.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_4.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_14.dds", 1, 1);

	m_pTextureManager->CreateResourceView(pd3dDevice, 0);

	m_pParticleShader = std::make_unique<CParticleShader>();
	m_pParticleShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteAttackObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::BillBoardTexture, L"Image/BillBoardImages/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
		m_pSpriteAttackObjects.push_back(std::move(m_pSpriteAttackObject));
	}

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CTerrainSpriteObject>(m_pTextureManager->LoadTextureIndex(TextureType::BillBoardTexture, L"Image/BillBoardImages/Circle1.dds"), pd3dDevice, pd3dCommandList, 0, 0, 15.f, 5.f);
		m_pTerrainSpriteObject.push_back(std::move(m_pSpriteObject));
	}

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pParticleObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/ParticleImages/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
		m_pParticleObjects.push_back(std::move(m_pParticleObject));
	}

	m_pSmokeObject = std::make_unique<CSmokeParticleObject>(L"Image/SmokeImages/Smoke2.dds", pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SMOKE_PARTICLE);

	m_pVertexPointParticleObject = std::make_unique<CVertexPointParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/ParticleImages/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.5f, 0.5f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
	//for (int i = 0; i < MAX_UPDOWN_PARTICLE_OBJECT; i++)
	//{
	//	std::unique_ptr<CParticleObject> m_pMeterorObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadParticleTexture(L"ParticleImage/Meteor.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), RECOVERY_PARTICLE);
	//	m_pUpDownParticleObjects.push_back(std::move(m_pMeterorObject));
	//}


	//m_pTrailParticleObjects = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/BillBoardImages/Effect3.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);

	CPlayerParticleObject::GetInst()->SetSmokeObjects(m_pSmokeObject.get());
	CPlayerParticleObject::GetInst()->SetTrailParticleObjects(m_pTrailParticleObjects.get());
	CPlayerParticleObject::GetInst()->SetVertexPointParticleObjects(m_pVertexPointParticleObject.get());

	// COLLIDE LISTENER
	std::unique_ptr<SceneCollideListener> pCollideListener = std::make_unique<SceneCollideListener>();
	pCollideListener->SetScene(this);
	m_pListeners.push_back(std::move(pCollideListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::COLLISION, m_pListeners.back().get(), nullptr);

	// ONGROUND LISTENER
	std::unique_ptr<SceneOnGroundListener> pOnGroundListener = std::make_unique<SceneOnGroundListener>();
	pOnGroundListener->SetScene(this);
	m_pListeners.push_back(std::move(pOnGroundListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::ONGROUND, m_pListeners.back().get(), nullptr);

	m_pHDRComputeShader = std::make_unique<CHDRComputeShader>();
	m_pHDRComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());

	m_pBloomComputeShader = std::make_unique<CBloomShader>();
	m_pBloomComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());
	m_pBloomComputeShader->CreateBloomUAVResource(pd3dDevice, pd3dCommandList, 1920, 1080);

	m_pLensFlareShader = std::make_unique<CLensFlareShader>();
	m_pLensFlareShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pLensFlareShader->BuildObjects(pd3dDevice, pd3dCommandList);

	/*m_pSwordTrailShader = std::make_unique<CSwordTrailShader>();
	m_pSwordTrailShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);
	m_pSwordTrailShader->BuildObjects(pd3dDevice, pd3dCommandList);*/

	for (int i = 0; i < 5; ++i) {
		std::unique_ptr<CSwordTrailObject> pSwordtrail = std::make_unique<CSwordTrailObject>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pSwordTrailShader.get());
		m_pSwordTrailObjects.push_back(std::move(pSwordtrail));
	}

}
bool CMainTMPScene::ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed)
{
	POINT ptCursorPos;
	float cxDelta, cyDelta;

	if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::FLOATING_MODE) {
		if (GetCapture() == hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
	}

	else if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE) {
		m_ptOldCursorPos = POINT(m_ScreendRect.left + (m_ScreendRect.right - m_ScreendRect.left) / 2,
			m_ScreendRect.top + (m_ScreendRect.bottom - m_ScreendRect.top) / 2
		);

		if (hWnd == GetFocus())
		{
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 8.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 8.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
	}

	m_pCurrentCamera->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed);
	((CPlayer*)m_pPlayer)->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed, m_pCurrentCamera);

	return true;
}

void CMainTMPScene::UpdateObjects(float fTimeElapsed)
{
	if (m_pPlayer) {
		if (!dynamic_cast<CPlayer*>(m_pPlayer)->m_pSwordTrailReference)
			dynamic_cast<CPlayer*>(m_pPlayer)->m_pSwordTrailReference = m_pSwordTrailObjects.data();
	}

	AnimationCompParams animation_comp_params;
	animation_comp_params.pObjects = &m_pEnemys;
	animation_comp_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<AnimationCompParams>(MessageType::UPDATE_OBJECT, &animation_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

	m_pLight->Update((CPlayer*)m_pPlayer);

	for (int i = 0; i < m_pEnemys.size(); ++i) {
		m_pEnemys[i]->Update(fTimeElapsed);
		m_pcbMappedDisolveParams->dissolveThreshold[i] = m_pEnemys[i]->m_fDissolveThrethHold;
	}

	m_pMap->Update(fTimeElapsed);
	m_pCollisionChecker->UpdateObjects(fTimeElapsed);

	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Update(fTimeElapsed);
	}

	m_pPlayer->Update(fTimeElapsed);

	//static bool isSetVPObject = false;
	//if (!isSetVPObject) {
	//	CGameObject* obj = m_pPlayer->FindFrame("Sword_low");
	//	m_pVertexPointParticleObject->SetWorldMatrixReference(&obj->m_xmf4x4World);
	//	// body
	//	m_pVertexPointParticleObject->SetVertexPointsFromSkinnedSubeMeshToRandom(m_pPlayer->m_pSkinnedAnimationController->m_ppSkinnedMeshes[0], 6, m_pPlayer->m_pSkinnedAnimationController.get());
	//	//m_pVertexPointParticleObject->SetVertexPointsFromSkinnedMeshToRandom(m_pPlayer->m_pSkinnedAnimationController->m_ppSkinnedMeshes[0], m_pPlayer->m_pSkinnedAnimationController.get());
	//	//m_pVertexPointParticleObject->SetVertexPointsFromStaticMeshToUniform(obj->m_pMesh.get());
	//	//m_pVertexPointParticleObject->SetVertexPointsFromStaticMeshToRandom(obj->m_pMesh.get());
	//	isSetVPObject = true;
	//}

	// Update Camera
	XMFLOAT3 xmf3PlayerPos = XMFLOAT3{
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
		 m_pPlayer->GetPosition().y,
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43 };
	xmf3PlayerPos.y += 0.35f;

	m_pMainSceneCamera->Update(xmf3PlayerPos, fTimeElapsed);

	CameraUpdateParams camera_update_params;
	camera_update_params.pCamera = m_pMainSceneCamera.get();
	camera_update_params.pPlayer = m_pPlayer;
	camera_update_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<CameraUpdateParams>(MessageType::UPDATE_CAMERA, &camera_update_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

	PlayerParams player_params;
	player_params.pPlayer = m_pPlayer;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER, &player_params, nullptr);
}

#define WITH_LAG_DOLL_SIMULATION

void CMainTMPScene::Update(float fTimeElapsed)
{
#ifdef WITH_LAG_DOLL_SIMULATION
	static float SimulateElapsedTime = 0.0f;

	SimulateElapsedTime += fTimeElapsed;
	if (!b_simulation) {
		Locator.GetPxScene()->simulate(SimulateElapsedTime);
		SimulateElapsedTime = 0.0f;
		b_simulation = true;
	}
	if (Locator.GetPxScene()->fetchResults(false)) {
		RegisterArticulations();
		UpdateObjectArticulation();
		b_simulation = false;
	}

#endif // WITH_LAG_DOLL_SIMULATION
	UpdateObjects(fTimeElapsed);
	m_pLensFlareShader->UpdateFlareAlpha(fTimeElapsed);
}
void CMainTMPScene::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	m_pPostProcessShader->OnPrepareRenderTarget(pd3dCommandList, 1, pd3dRtvCPUHandles, d3dDepthStencilBufferDSVCPUHandle);
}
void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	m_pCurrentCamera->OnPrepareRender(pd3dCommandList);

	m_pLight->Render(pd3dCommandList);

	m_pSunLightShader->Render(pd3dCommandList, m_pCurrentCamera);

	m_pSkyBoxShader->Render(pd3dCommandList, 0);
	m_pSkyBoxObject->Render(pd3dCommandList, m_pCurrentCamera);

	m_pMap->RenderTerrain(pd3dCommandList);
	
	((CParticleObject*)m_pSmokeObject.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	
	//((CParticleObject*)m_pSmokeObject.get())->Update(fTimeElapsed);
	//((CParticleObject*)m_pSmokeObject.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//((CParticleObject*)m_pSmokeObject.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	

	//for (int i = 0; i < m_pUpDownParticleObjects.size(); ++i)
	//{
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Update(fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	//}
	
	/*((CParticleObject*)m_pTrailParticleObjects.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	*/
	//m_pBillBoardObjectShader->Render(pd3dCommandList, 0);
	for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		//(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		//(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->Animate(m_pTerrain.get(), fTimeElapsed);
		//m_pTerrainSpriteObject[i]->Render(pd3dCommandList, true);

		((CParticleObject*)m_pTerrainSpriteObject[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->Animate(fTimeElapsed);
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}


	CModelShader::GetInst()->Render(pd3dCommandList, 0);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbDisolveParams->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);

	m_pMap->RenderMapObjects(pd3dCommandList);

	if (m_pPlayer)
	{
		//m_pPlayer->Animate(0.0f);
		m_pPlayer->Render(pd3dCommandList, true);
	}

	static float trailUpdateT = 0.0f;
	trailUpdateT += fTimeElapsed;
	if (trailUpdateT > 0.008f) {
		for (int i = 0; i < m_pSwordTrailObjects.size(); ++i) {
			CSwordTrailObject* trailObj = dynamic_cast<CSwordTrailObject*>(m_pSwordTrailObjects[i].get());
			switch (trailObj->m_eTrailUpdateMethod)
			{
			case TRAIL_UPDATE_METHOD::UPDATE_NEW_CONTROL_POINT:
				trailObj->SetNextControllPoint(&((CPlayer*)m_pPlayer)->GetTrailControllPoint(0), &((CPlayer*)m_pPlayer)->GetTrailControllPoint(1));
				break;
			case TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT:
				break;
			case TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT:
				trailObj->SetNextControllPoint(nullptr, nullptr);

				break;
			default:
				break;
			}
		}
		trailUpdateT = 0.0f;

		static int count = 100;
		count--;
		if (count < 1) {

			count = 100;
		}
	}

	//m_pVertexPointParticleObject->EmitParticle(5);
	//m_pVertexPointParticleObject->SetEmit(true);

	UINT index = 0;
	for (int i = 0; i < m_pEnemys.size(); ++i)
	{
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &i, 33);
		m_pEnemys[i]->Animate(0.0f);
		m_pEnemys[i]->Render(pd3dCommandList, true);
	}


#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
#endif

#define PostProcessing

#ifdef PostProcessing
	m_pPostProcessShader->Render(pd3dCommandList, pCamera);
#endif // PostProcessing

	m_pTextureManager->SetTextureDescriptorHeap(pd3dCommandList);
	m_pTextureManager->UpdateShaderVariables(pd3dCommandList);


	for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->Animate(m_pMap->GetTerrain().get(), fTimeElapsed);
		m_pTerrainSpriteObject[i]->Render(pd3dCommandList, true);
	}
	
	

	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Animate(fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}

	for (int i = 0; i < m_pSpriteAttackObjects.size(); ++i)
	{
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Animate(fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}
	//((CParticleObject*)m_pTrailParticleObjects.get())->Update(fTimeElapsed);
	//((CParticleObject*)m_pTrailParticleObjects.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//((CParticleObject*)m_pTrailParticleObjects.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	((CParticleObject*)m_pSmokeObject.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	//m_pVertexPointParticleObject->Update(fTimeElapsed);
	//m_pVertexPointParticleObject->Animate(fTimeElapsed);
	//m_pVertexPointParticleObject->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//m_pVertexPointParticleObject->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	/*m_pSwordTrailShader->Render(pd3dCommandList, pCamera, 0);*/
	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Render(pd3dCommandList, true);
	}

	m_pLensFlareShader->CalculateFlaresPlace(m_pCurrentCamera, &m_pLight->GetLights()[0]);
	m_pLensFlareShader->Render(pd3dCommandList, 0);

	if (m_pd3dComputeRootSignature) pd3dCommandList->SetComputeRootSignature(m_pd3dComputeRootSignature.Get());
	ID3D12Resource* pd3dSource;
	ID3D12Resource* pd3dDestination;

	{
		m_pBloomComputeShader->Dispatch(pd3dCommandList);

		pd3dSource = m_pBloomComputeShader->m_pBloomedTexture->GetResource(0);
		pd3dDestination = m_pHDRComputeShader->m_pSourceTexture->GetResource(0);

		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		pd3dCommandList->CopyResource(pd3dDestination, pd3dSource);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	}


	m_pHDRComputeShader->Dispatch(pd3dCommandList);

	pd3dSource = m_pHDRComputeShader->m_pTextures->GetResource(0);
	pd3dDestination = m_pHDRComputeShader->m_pRenderTargetResource;

	::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	pd3dCommandList->CopyResource(pd3dDestination, pd3dSource);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void CMainTMPScene::OnPostRender()
{
	m_pCurrentCamera->OnPostRender();

	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->OnPostRender();
	}

	((CParticleObject*)m_pSmokeObject.get())->OnPostRender();

	for (int i = 0; i < m_pSpriteAttackObjects.size(); ++i)
	{
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->OnPostRender();
	}

	//((CParticleObject*)m_pTrailParticleObjects.get())->OnPostRender();
	//m_pVertexPointParticleObject.get()->OnPostRender();
}

void CMainTMPScene::HandleCollision(const CollideParams& params)
{
	std::vector<std::unique_ptr<CGameObject>>::iterator it = std::find_if(m_pParticleObjects.begin(), m_pParticleObjects.end(), [](const std::unique_ptr<CGameObject>& pParticleObject) {
		if (((CParticleObject*)pParticleObject.get())->CheckCapacity())
			return true;
		return false;
		});

	if (it != m_pParticleObjects.end())
	{
		ParticleCompParams particle_comp_params;
		particle_comp_params.pObject = (*it).get();
		particle_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_PARTICLE, &particle_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
	}

	it = std::find_if(m_pSpriteAttackObjects.begin(), m_pSpriteAttackObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
		if (((CParticleObject*)pBillBoardObject.get())->CheckCapacity())
			return true;
		return false;
		});


	if (it != m_pSpriteAttackObjects.end())
	{
		ImpactCompParams impact_comp_params;
		impact_comp_params.pObject = (*it).get();
		impact_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<ImpactCompParams>(MessageType::UPDATE_BILLBOARD, &impact_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
		CLogger::GetInst()->Log(std::string("Display Impact!!!!!!!!!!!!!"));
	}

	// Demo 사용x
	//it = std::find_if(m_pUpDownParticleObjects.begin(), m_pUpDownParticleObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
	//	if (((CParticleObject*)pBillBoardObject.get())->CheckCapacity())
	//		return true;
	//	return false;
	//	});

	//if (it != m_pUpDownParticleObjects.end())
	//{
	//	ParticleUpDownParams particleUpdown_comp_params;
	//	particleUpdown_comp_params.pObject = (*it).get();
	//	particleUpdown_comp_params.xmf3Position = params.xmf3CollidePosition;
	//	CMessageDispatcher::GetInst()->Dispatch_Message<ParticleUpDownParams>(MessageType::UPDATE_UPDOWNPARTICLE, &particleUpdown_comp_params, Damaged_Monster::GetInst());
	//}

	//std::vector<std::unique_ptr<CGameObject>>::iterator TerrainSpriteit = std::find_if(m_pTerrainSpriteObject.begin(), m_pTerrainSpriteObject.end(), [](const std::unique_ptr<CGameObject>& pTerrainSpriteObject) {
	//	if (((CParticleObject*)pTerrainSpriteObject.get())->CheckCapacity())
	//		return true;
	//	return false;
	//	});

	//if (TerrainSpriteit != m_pTerrainSpriteObject.end())
	//{
	//	TerrainSpriteCompParams AttackSprite_comp_params;
	//	AttackSprite_comp_params.pObject = (*TerrainSpriteit).get();
	//	AttackSprite_comp_params.xmf3Position = params.xmf3CollidePosition;
	//	CMessageDispatcher::GetInst()->Dispatch_Message<TerrainSpriteCompParams>(MessageType::UPDATE_SPRITE, &AttackSprite_comp_params, Spawn_Monster::GetInst());
	//	TCHAR pstrDebug[256] = { 0 };
	//	_stprintf_s(pstrDebug, 256, _T("바닥 충격 출력 %f, %f, %f\n"), params.xmf3CollidePosition.x, params.xmf3CollidePosition.y, params.xmf3CollidePosition.z);
	//	OutputDebugString(pstrDebug);
	//}
	//else
	//{
	//	TCHAR pstrDebug[256] = { 0 };
	//	_stprintf_s(pstrDebug, 256, _T("No바닥 충격 출력 %f, %f, %f\n"), params.xmf3CollidePosition.x, params.xmf3CollidePosition.y, params.xmf3CollidePosition.z);
	//	OutputDebugString(pstrDebug);
	//}



}

void CMainTMPScene::HandleOnGround(const OnGroundParams& params)
{
	std::vector<std::unique_ptr<CGameObject>>::iterator TerrainSpriteit = std::find_if(m_pTerrainSpriteObject.begin(), m_pTerrainSpriteObject.end(), [](const std::unique_ptr<CGameObject>& pTerrainSpriteObject) {
		if (((CParticleObject*)pTerrainSpriteObject.get())->CheckCapacity())
			return true;
		return false;
		});

	if (TerrainSpriteit != m_pTerrainSpriteObject.end())
	{
		TerrainSpriteCompParams AttackSprite_comp_params;
		AttackSprite_comp_params.pObject = (*TerrainSpriteit).get();
		AttackSprite_comp_params.xmf3Position = params.xmf3OnGroundPosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<TerrainSpriteCompParams>(MessageType::UPDATE_SPRITE, &AttackSprite_comp_params, Spawn_Monster::GetInst());
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("바닥 충격 출력 %f, %f, %f\n"), params.xmf3OnGroundPosition.x, params.xmf3OnGroundPosition.y, params.xmf3OnGroundPosition.z);
		OutputDebugString(pstrDebug);
	}
	else
	{
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("No바닥 충격 출력 %f, %f, %f\n"), params.xmf3OnGroundPosition.x, params.xmf3OnGroundPosition.y, params.xmf3OnGroundPosition.z);
		OutputDebugString(pstrDebug);
	}
}
