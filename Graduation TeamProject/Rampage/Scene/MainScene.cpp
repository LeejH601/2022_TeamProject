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
#include "..\Global\Locator.h"

#define MAX_PARTICLE_OBJECT 50
#define MAX_ATTACKSPRITE_OBJECT 50
#define MAX_TERRAINSPRITE_OBJECT 50

void CMainTMPScene::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
	((CPlayer*)m_pPlayer)->SetUpdatedContext(m_pTerrain.get());
	((CPlayer*)m_pPlayer)->SetCamera(m_pMainSceneCamera.get());
	((CThirdPersonCamera*)m_pMainSceneCamera.get())->SetPlayer((CPlayer*)m_pPlayer);

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
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[6];
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
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 30; //t30: gtxtTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 3;
	pd3dDescriptorRanges[4].BaseShaderRegister = 31; //t31~33: gtxtParticleTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 7;
	pd3dDescriptorRanges[5].BaseShaderRegister = 35; //t35: gtxMultiRenderTargetTextures
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[14];

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
	pd3dRootParameters[7].Descriptor.ShaderRegister = 5; // b5 : Parallex constant
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[5];
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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
SCENE_RETURN_TYPE CMainTMPScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_BACK:
			return SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE;
		case '1':
			m_pCurrentCamera = m_pFloatingCamera.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
			break;
		case '2':
			m_pCurrentCamera = m_pMainSceneCamera.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::THIRD_FERSON_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
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

	UINT ncbElementBytes = ((sizeof(DissolveParams) + 255) & ~255); //256의 배수
	m_pd3dcbDisolveParams = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbDisolveParams->Map(0, NULL, (void**)&m_pcbMappedDisolveParams);

	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT };

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

	std::unique_ptr<CGoblinObject> m_pGoblinObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblinObject->SetPosition(XMFLOAT3(200, 300, -120));
	m_pGoblinObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pGoblinObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pGoblinObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pGoblinObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_pObjects.push_back(std::move(m_pGoblinObject));

	m_pGoblinObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblinObject->SetPosition(XMFLOAT3(200, 300, -60));
	m_pGoblinObject->SetScale(4.0f, 4.0f, 4.0f);
	m_pGoblinObject->Rotate(0.0f, 180.0f, 0.0f);
	m_pGoblinObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pObjects.push_back(std::move(m_pGoblinObject));

	std::unique_ptr<CGoblinObject> m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);

	{
		m_pGoblin->SetPosition(XMFLOAT3(150, 300, -120));
		m_pGoblin->SetScale(4.0f, 4.0f, 4.0f);
		m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
		m_pGoblin->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
		m_pGoblin->CreateArticulation(1.0f);
		m_pGoblin->m_bSimulateArticulate = false;
		m_pGoblin->Animate(0.0f);
		m_pGoblin->OnPrepareRender();
		m_pGoblin->m_bSimulateArticulate = true;
		XMFLOAT4X4 rootWorld = m_pGoblin->FindFrame("root")->m_xmf4x4World;
		m_pGoblin->m_pArticulation->copyInternalStateToCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		physx::PxTransform* rootLInkTrans = &m_pGoblin->m_pArticulationCache->rootLinkData->transform;
		physx::PxMat44 tobonetrans = physx::PxMat44(*rootLInkTrans);
		tobonetrans.column3.x += rootWorld._41;
		tobonetrans.column3.y += rootWorld._42;
		tobonetrans.column3.z += rootWorld._43;
		*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
		m_pGoblin->m_pArticulation->applyCache(*m_pGoblin->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);

		m_pObjects.push_back(std::move(m_pGoblin));
	}


	std::unique_ptr<COrcObject> m_pOrc = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
	m_pOrc->SetPosition(XMFLOAT3(150, 300, -90));
	m_pOrc->SetScale(4.0f, 4.0f, 4.0f);
	m_pOrc->Rotate(0.0f, 0.0f, 0.0f);
	m_pOrc->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pOrc->CreateArticulation(1.0f);
	m_pOrc->m_bSimulateArticulate = false;
	m_pOrc->Animate(0.0f);
	m_pOrc->OnPrepareRender();
	m_pOrc->m_bSimulateArticulate = true;
	physx::PxMat44 mat = m_pOrc->m_pArticulation->getRootGlobalPose();
	XMFLOAT4X4 rootWorld = m_pOrc->FindFrame("root")->m_xmf4x4World;
	XMFLOAT4X4 gobworld = m_pOrc->FindFrame("pelvis")->m_xmf4x4World;
	m_pOrc->m_pArticulation->copyInternalStateToCache(*m_pOrc->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	physx::PxTransform* rootLInkTrans = &m_pOrc->m_pArticulationCache->rootLinkData->transform;
	physx::PxMat44 tobonetrans = physx::PxMat44(*rootLInkTrans);
	tobonetrans.column3.x += rootWorld._41;
	tobonetrans.column3.y += rootWorld._42;
	tobonetrans.column3.z += rootWorld._43;
	*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
	m_pOrc->m_pArticulation->applyCache(*m_pOrc->m_pArticulationCache, physx::PxArticulationCacheFlag::eROOT_TRANSFORM);

	m_pObjects.push_back(std::move(m_pOrc));

	m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pGoblin->SetPosition(XMFLOAT3(150, 300, -150));
	m_pGoblin->SetScale(4.0f, 4.0f, 4.0f);
	m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
	m_pGoblin->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pGoblin->CreateArticulation(1.0f);
	m_pGoblin->m_bSimulateArticulate = false;
	m_pGoblin->Animate(0.0f);
	m_pGoblin->OnPrepareRender();
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
	m_pGoblin->SetPosition(XMFLOAT3(150, 300, -180));
	m_pGoblin->SetScale(4.0f, 4.0f, 4.0f);
	m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
	m_pGoblin->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pGoblin->CreateArticulation(1.0f);
	m_pGoblin->m_bSimulateArticulate = false;
	m_pGoblin->Animate(0.0f);
	m_pGoblin->OnPrepareRender();
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
	for (int i = 0; i < 0; ++i) {
		testpos.x -= 25.0f;
		m_pGoblin = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
		m_pGoblin->SetPosition(testpos);
		m_pGoblin->SetScale(14.0f, 14.0f, 14.0f);
		m_pGoblin->Rotate(0.0f, 0.0f, 0.0f);
		m_pGoblin->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
		m_pGoblin->CreateArticulation(1.0f);
		m_pGoblin->m_bSimulateArticulate = false;
		m_pGoblin->Animate(0.0f);
		m_pGoblin->OnPrepareRender();
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

	// Light 생성
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);


	// Terrain 생성
	XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Terrain/terrainHeightMap_5.raw"), 513, 513, 513, 513, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	float minHeight = FLT_MAX;
	for (int x = 0; x < m_pTerrain->GetWidth(); ++x) {
		for (int z = 0; z < m_pTerrain->GetLength(); ++z) {
			if (minHeight > m_pTerrain->GetHeight(x, z)) {
				minHeight = m_pTerrain->GetHeight(x, z);
			}
		}
	}
	m_pTerrain->SetPosition(XMFLOAT3(86.4804, -46.8876 - 46.8876 * 0.38819 + 6.5f, -183.7856));
	m_pTerrain->SetRigidStatic();
	LoadSceneFromFile(pd3dDevice, pd3dCommandList, "Object/Scene/Scene.bin");
	m_IObjectIndexs.resize(m_pObjects.size());
	for (int i = 0; i < m_pObjects.size(); ++i) {
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pObjects[i].get());

		if (dynamic_cast<CPhysicsObject*>(m_pObjects[i].get()))
			((CPhysicsObject*)m_pObjects[i].get())->SetUpdatedContext(m_pTerrain.get());

		m_IObjectIndexs[i] = i;
	}
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetLight(m_pLight->GetLights());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetTerrain(m_pTerrain.get());

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

	m_pBillBoardObjectShader = std::make_unique<CBillBoardObjectShader>();
	m_pBillBoardObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pBillBoardObjectShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 10);

	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds", m_pBillBoardObjectShader.get(), 8, 8);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Fire_Effect.dds", m_pBillBoardObjectShader.get(), 5, 6);

	for (int i = 0; i < 50; ++i)
	{
		std::unique_ptr<CGameObject> m_pBillBoardObject = std::make_unique<CMultiSpriteObject>(m_pTextureManager->LoadBillBoardTexture(L"Image/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 5, 6, 8.f, 5.f);
		m_pBillBoardObjects.push_back(std::move(m_pBillBoardObject));
	}
	
	m_pParticleShader = std::make_unique<CParticleShader>();
	m_pParticleShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 40);
	m_pParticleShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);

	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"ParticleImage/RoundSoftParticle.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"ParticleImage/Smoke.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"ParticleImage/Smoke2.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect0.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect1.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect2.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect3.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect4.dds", m_pParticleShader.get(), 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect5.dds", m_pParticleShader.get(), 0, 0);

	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds", m_pBillBoardObjectShader.get(), 8, 8);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Fire_Effect.dds", m_pBillBoardObjectShader.get(), 5, 6);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Circle0.dds", m_pBillBoardObjectShader.get(), 0, 0);

	for (int i = 0; i < MAX_PARTICLE_OBJECT; ++i) 
	{
		std::unique_ptr<CGameObject> m_pParticleObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadParticleTexture(L"ParticleImage/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get());
		m_pParticleObjects.push_back(std::move(m_pParticleObject));
	}


	for (int i = 0; i < MAX_ATTACKSPRITE_OBJECT; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CMultiSpriteObject>(m_pTextureManager->LoadBillBoardTexture(L"Image/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 5, 6, 8.f, true, 5.f);
		m_pSpriteAttackObjects.push_back(std::move(m_pSpriteObject));
	}

	for (int i = 0; i < MAX_TERRAINSPRITE_OBJECT; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CTerrainSpriteObject>(m_pTextureManager->LoadBillBoardTexture(L"Image/Circle0.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 0, 0, 15.f, 5.f);
		m_pTerrainSpriteObject.push_back(std::move(m_pSpriteObject));
	}

	m_pSmokeObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadParticleTexture(L"ParticleImage/Smoke2.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get());
	((CParticleObject*)m_pSmokeObject.get())->SetEnable(true);

	// COLLIDE LISTENER
	std::unique_ptr<SceneCollideListener> pCollideListener = std::make_unique<SceneCollideListener>();
	pCollideListener->SetScene(this);
	m_pListeners.push_back(std::move(pCollideListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::COLLISION, m_pListeners.back().get(), nullptr);

	m_pHDRComputeShader = std::make_unique<CHDRComputeShader>();
	m_pHDRComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());

	m_pBloomComputeShader = std::make_unique<CBloomShader>();
	m_pBloomComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());
	m_pBloomComputeShader->CreateBloomUAVResource(pd3dDevice, pd3dCommandList, 1920, 1080);
}
bool CMainTMPScene::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
	m_pCurrentCamera->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed);
	((CPlayer*)m_pPlayer)->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed, m_pCurrentCamera, (CParticleObject*)m_pSmokeObject.get());
	((CParticleObject*)m_pSmokeObject.get())->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed, m_pCurrentCamera);
	return true;
}

void CMainTMPScene::UpdateObjects(float fTimeElapsed)
{
	AnimationCompParams animation_comp_params;
	animation_comp_params.pObjects = &m_pObjects;
	animation_comp_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<AnimationCompParams>(MessageType::UPDATE_OBJECT, &animation_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

	m_pLight->Update((CPlayer*)m_pPlayer);

	for (int i = 0; i < m_pObjects.size(); ++i) {
		m_pObjects[i]->Update(fTimeElapsed);
		m_pcbMappedDisolveParams->dissolveThreshold[i] = m_pObjects[i]->m_fDissolveThrethHold;
	}

	for (int i = 0; i < m_pBillBoardObjects.size(); ++i)
	{
		m_pBillBoardObjects[i]->Animate(fTimeElapsed);
	}

	m_pPlayer->Update(fTimeElapsed);

	// Update Camera
	XMFLOAT3 xmf3PlayerPos = m_pPlayer->GetPosition();
	xmf3PlayerPos.y += 12.5f;

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
		UpdateObjectArticulation();
		b_simulation = false;
	}

#endif // WITH_LAG_DOLL_SIMULATION
	UpdateObjects(fTimeElapsed);
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

	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);

	m_pBillBoardObjectShader->Render(pd3dCommandList, 0);

	for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		m_pTerrainSpriteObject[i]->Render(pd3dCommandList, true);
	}

	for (int i = 0; i < m_pBillBoardObjects.size(); ++i)
		m_pBillBoardObjects[i]->Render(pd3dCommandList, true);

	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}

	CModelShader::GetInst()->Render(pd3dCommandList, 0);

	if (m_pPlayer)
	{
		m_pPlayer->Animate(0.0f);
		m_pPlayer->Render(pd3dCommandList, true);
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbDisolveParams->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);

	UINT index = 0;
	for (int i = 0; i < m_pObjects.size(); ++i)
	{
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &i, 33);
		m_pObjects[i]->Animate(0.0f);
		m_pObjects[i]->Render(pd3dCommandList, true);
	}

#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
#endif

#define PostProcessing

#ifdef PostProcessing
	m_pPostProcessShader->Render(pd3dCommandList, pCamera);
#endif // PostProcessing

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
}

void CMainTMPScene::LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CMainTMPScene* pScene = this;

	char pstrToken[64] = { '\0' };

	int nGameObjects;

	UINT nRead;

	::ReadStringFromFile(pInFile, pstrToken);

	if (!strcmp(pstrToken, "<GameObjects>:"))
	{
		int nObjects = ReadIntegerFromFile(pInFile);
		for (int i = 0; i < nObjects; ++i) {
			ReadStringFromFile(pInFile, pstrToken);
			nRead = ReadStringFromFile(pInFile, pstrToken);

			float buffer[16];
			std::string objPath{ "Object/Scene/" };
			objPath += static_cast<std::string>(pstrToken) + ".bin";
			FILE* objFile = NULL;
			::fopen_s(&objFile, objPath.data(), "rb");
			::rewind(objFile);

			std::unique_ptr<CGameObject> pObject = std::make_unique<CGameObject>();

			nRead = (UINT)::fread(&buffer, sizeof(float), 16, pInFile);


			XMFLOAT4X4 xmfWorld = {
				buffer[0],buffer[1],buffer[2],buffer[3],
				buffer[4],buffer[5],buffer[6],buffer[7],
				buffer[8],buffer[9],buffer[10],buffer[11],
				buffer[12],buffer[13],buffer[14],buffer[15]
			};

			CLoadedModelInfo* rootObj = CModelManager::GetInst()->LoadGeometryFromFileOfScene(pd3dDevice, pd3dCommandList, objFile);
			rootObj->m_pModelRootObject->m_xmf4x4Transform = xmfWorld;

			std::string name{ pstrToken };
			if (name.find("Tree") != std::string::npos || name.find("Rock") != std::string::npos)
			{
				physx::PxTolerancesScale scale = Locator.GetPxPhysics()->getTolerancesScale();
				physx::PxCookingParams params(scale);
				
				params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

				physx::PxTriangleMeshDesc meshDesc;
				std::vector<XMFLOAT3> vertexs = rootObj->m_pModelRootObject->m_pMesh->GetVertexs();
				std::vector<XMFLOAT3> world_vertexs; world_vertexs.resize(vertexs.size());
				XMMATRIX trans = XMLoadFloat4x4(&rootObj->m_pModelRootObject->m_xmf4x4Transform);

				XMVECTOR scaling;
				XMVECTOR rotation;
				XMVECTOR translation;
				XMMatrixDecompose(&scaling, &rotation, &translation, XMLoadFloat4x4(&rootObj->m_pModelRootObject->m_xmf4x4Transform));
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

				std::vector<UINT> Indices = rootObj->m_pModelRootObject->m_pMesh->GetIndices();
				meshDesc.triangles.count = Indices.size() / 3;
				meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
				meshDesc.triangles.data = Indices.data();


				physx::PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc, Locator.GetPxPhysics()->getPhysicsInsertionCallback());

				physx::PxTransform transform = physx::PxTransform(buffer[12], buffer[13], buffer[14]);
				transform.q.normalize();
				physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.5, 0.5, 0.5);

				physx::PxRigidStatic* actor = physx::PxCreateStatic(*Locator.GetPxPhysics(), transform, physx::PxTriangleMeshGeometry(aTriangleMesh), *material);
				Locator.GetPxScene()->addActor(*actor);

			}

			pObject->SetChild(rootObj->m_pModelRootObject, true);
			XMFLOAT4X4 matrix_scale = {
					10, 0, 0, 0,
					0, 10, 0, 0,
					0, 0, 10, 0,
					0, 0, 0, 1,
			};
			pObject->UpdateTransform(NULL);

			m_pObjects.push_back(std::move(pObject));

			fclose(objFile);
		}
	}
}

void CMainTMPScene::HandleCollision(const CollideParams& params)
{
	std::vector<std::unique_ptr<CGameObject>>::iterator it = std::find_if(m_pParticleObjects.begin(), m_pParticleObjects.end(), [](const std::unique_ptr<CGameObject>& pParticleObject) {
		if (!((CParticleObject*)pParticleObject.get())->GetEnable())
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

	it = std::find_if(m_pBillBoardObjects.begin(), m_pBillBoardObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
		if (!((CParticleObject*)pBillBoardObject.get())->GetEnable())
			return true;
		return false;
		});

	if (it != m_pBillBoardObjects.end())
	{
		ImpactCompParams impact_comp_params;
		impact_comp_params.pObject = (*it).get();
		impact_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<ImpactCompParams>(MessageType::UPDATE_BILLBOARD, &impact_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
	}

	std::vector<std::unique_ptr<CGameObject>>::iterator TerrainSpriteit = std::find_if(m_pTerrainSpriteObject.begin(), m_pTerrainSpriteObject.end(), [](const std::unique_ptr<CGameObject>& pSpriteAttackObject) {
		if (!((CMultiSpriteObject*)pSpriteAttackObject.get())->GetEnable())
			return true;
		return false;
		});

	if (TerrainSpriteit != m_pTerrainSpriteObject.end())
	{
		TerrainSpriteCompParams AttackSprite_comp_params;
		AttackSprite_comp_params.pObject = (*TerrainSpriteit).get();
		AttackSprite_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<TerrainSpriteCompParams>(MessageType::UPDATE_SPRITE, &AttackSprite_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
	}
}


