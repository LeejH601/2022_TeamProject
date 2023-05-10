#include "SimulatorScene.h"
#include "..\Global\Timer.h"
#include "..\Object\Texture.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Sound\SoundManager.h"
#include "..\Sound\SoundPlayer.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Shader\DepthRenderShader.h"
#include "..\Object\TextureManager.h"
#include "..\Shader\PostProcessShader.h"
#include "..\Object\MonsterState.h"
#include "..\Object\SwordTrailObject.h"

void CSimulatorScene::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (m_pDepthRenderShader)
	{
		((CDepthRenderShader*)m_pDepthRenderShader.get())->PrepareShadowMap(pd3dCommandList, 0.0f);
		((CDepthRenderShader*)m_pDepthRenderShader.get())->UpdateDepthTexture(pd3dCommandList);
	}
}
void CSimulatorScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CSimulatorScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
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
	pd3dDescriptorRanges[3].NumDescriptors = 30;
	pd3dDescriptorRanges[3].BaseShaderRegister = 50; //t50: gtxtTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 2;
	pd3dDescriptorRanges[4].BaseShaderRegister = 31; //t31~32: gtxtParticleTexture
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
void CSimulatorScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
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
void CSimulatorScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);
	CreateComputeRootSignature(pd3dDevice);

	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	XMFLOAT3 offset{ 86.4804 , 0.0f, -183.7856 };

	m_pSimulaterCamera = std::make_unique<CSimulatorCamera>();
	m_pSimulaterCamera->Init(pd3dDevice, pd3dCommandList);
	m_pSimulaterCamera->SetPosition(XMFLOAT3(43 + offset.x, 62, 46 + offset.z));
	m_pSimulaterCamera->SetLookAt(XMFLOAT3(100 + offset.x, 0, 100 + offset.z));
	m_pSimulaterCamera->RegenerateViewMatrix();

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthRenderShader = std::make_unique<CDepthRenderShader>();
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_pDepthRenderShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture()->GetTextures());
	m_pDepthRenderShader->CreateShaderResourceViews(pd3dDevice, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture(), 0, 9);

	// Light 持失
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);

	// 4->HIT
	// 5->IDLE
	std::unique_ptr<CMonster> m_pDummyEnemy = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
	m_pDummyEnemy->SetPosition(XMFLOAT3(50 + offset.x, 100, 50 + offset.z));
	m_pDummyEnemy->SetScale(4.0f, 4.0f, 4.0f);
	m_pDummyEnemy->Rotate(0.0f, -90.0f, 0.0f);
	m_pDummyEnemy->m_fHP = FLT_MAX;
	m_pDummyEnemy->m_bIsDummy = true;
	m_pDummyEnemy->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	m_pEnemys.push_back(std::move(m_pDummyEnemy));

	// 3->IDLE
	// 28->Attack
	m_pMainCharacter = std::make_unique<CKnightPlayer>(pd3dDevice, pd3dCommandList, 1);
	m_pMainCharacter->SetCamera(m_pSimulaterCamera.get());
	m_pMainCharacter->SetPosition(XMFLOAT3(45 + offset.x, 100, 50 + offset.z));
	m_pMainCharacter->SetScale(4.0f, 4.0f, 4.0f);
	m_pMainCharacter->Rotate(0.0f, 90.0f, 0.0f);
	m_pMainCharacter->m_pSkinnedAnimationController->m_bRootMotion = false;
	m_pMainCharacter->m_pStateMachine->ChangeState(Idle_Player::GetInst());

	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetLight(m_pLight->GetLights());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetTerrain(m_pTerrain.get());

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// Terrain 持失
	XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), _T("Terrain/terrainHeightMap_5.raw"), 513, 513, 513, 513, xmf3Scale, xmf4Color, m_pTerrainShader.get());
	m_pTerrain->SetPosition(XMFLOAT3(offset.x, 0, offset.z));

	((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pMainCharacter.get());
	m_pMainCharacter->SetUpdatedContext(m_pTerrain.get());

	for (int i = 0; i < m_pEnemys.size(); ++i)
	{
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pEnemys[i].get());
		((CMonster*)m_pEnemys[i].get())->SetUpdatedContext(m_pTerrain.get());
	}

	m_pTextureManager = std::make_unique<CTextureManager>();
	m_pTextureManager->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 100);
	m_pTextureManager->LoadSphereBuffer(pd3dDevice, pd3dCommandList);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds", 8, 8);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Fire_Effect.dds", 5, 6);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds", 8, 8);
	m_pTextureManager->LoadBillBoardTexture(pd3dDevice, pd3dCommandList, L"Image/Circle1.dds", 1, 1);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"ParticleImage/RoundSoftParticle.dds", 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"ParticleImage/Meteor.dds", 0, 0);
	m_pTextureManager->LoadParticleTexture(pd3dDevice, pd3dCommandList, L"Image/Effect0.dds", 0, 0);
	
	m_pTextureManager->CreateResourceView(pd3dDevice, 0);

	m_pBillBoardObjectShader = std::make_unique<CBillBoardObjectShader>();
	m_pBillBoardObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);


	for (int i = 0; i < 50; ++i)
	{
		std::unique_ptr<CGameObject> m_pBillBoardObject = std::make_unique<CMultiSpriteObject>(m_pTextureManager->LoadTextureIndex(L"Image/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 5, 6, 8.f, 5.f);
		m_pBillBoardObjects.push_back(std::move(m_pBillBoardObject));
	}

	m_pParticleShader = std::make_unique<CParticleShader>();
	//m_pParticleShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 60);
	m_pParticleShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);

	
	for (int i = 0; i < MAX_PARTICLE_OBJECT; ++i)
	{
		std::unique_ptr<CGameObject> m_pParticleObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(L"ParticleImage/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
		m_pParticleObjects.push_back(std::move(m_pParticleObject));
	}

	//for (int i = 0; i < MAX_RECOVERYPARTICLE_OBJECT; ++i)
	//{
	//	std::unique_ptr<CGameObject> m_pParticleObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadParticleTexture(L"ParticleImage/Meteor.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), RECOVERY_PARTICLE);
	//	m_pUpDownParticleObjects.push_back(std::move(m_pParticleObject));
	//}
	
	for (int i = 0; i < MAX_ATTACKSPRITE_OBJECT; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CMultiSpriteObject>(m_pTextureManager->LoadTextureIndex(L"Image/Fire_Effect.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 5, 6, 8.f, true, 5.f);
		m_pSpriteAttackObjects.push_back(std::move(m_pSpriteObject));
	}

	for (int i = 0; i < MAX_TERRAINSPRITE_OBJECT; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CTerrainSpriteObject>(m_pTextureManager->LoadTextureIndex(L"Image/Circle1.dds"), pd3dDevice, pd3dCommandList, m_pBillBoardObjectShader.get(), 0, 0, 15.f, 5.f);
		m_pTerrainSpriteObject.push_back(std::move(m_pSpriteObject));
	}

	m_pPostProcessShader = std::make_unique<CPostProcessShader>();
	m_pPostProcessShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pPostProcessShader->BuildObjects(pd3dDevice, pd3dCommandList);

	// COLLIDE LISTENER
	std::unique_ptr<SceneCollideListener> pCollideListener = std::make_unique<SceneCollideListener>();
	pCollideListener->SetScene(this);
	m_pListeners.push_back(std::move(pCollideListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::COLLISION, m_pListeners.back().get(), nullptr);

	m_pHDRComputeShader = std::make_unique<CHDRComputeShader>();
	//m_pHDRComputeShader->SetTextureSource(m_pPostProcessShader->GetTextureShared());
	m_pHDRComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());

	m_pBloomComputeShader = std::make_unique<CBloomShader>();
	m_pBloomComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());
	m_pBloomComputeShader->CreateBloomUAVResource(pd3dDevice, pd3dCommandList, 1920, 1080);

	m_pSwordTrailShader = std::make_unique<CSwordTrailShader>();
	m_pSwordTrailShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);
	m_pSwordTrailShader->BuildObjects(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < 5; ++i) {
		std::unique_ptr<CSwordTrailObject> pSwordtrail = std::make_unique<CSwordTrailObject>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pSwordTrailShader.get());
		m_pSwordTrailObjects.push_back(std::move(pSwordtrail));
	}

	m_pMainCharacter->m_pSwordTrailReference = m_pSwordTrailObjects.data();
}
void CSimulatorScene::Update(float fTimeElapsed)
{
	UpdateObjects(fTimeElapsed);
}

void CSimulatorScene::UpdateObjects(float fTimeElapsed)
{
	AnimationCompParams animation_comp_params;
	animation_comp_params.pObjects = &m_pEnemys;
	animation_comp_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<AnimationCompParams>(MessageType::UPDATE_OBJECT, &animation_comp_params, m_pMainCharacter->m_pStateMachine->GetCurrentState());
	
	for (int i = 0; i < m_pEnemys.size(); ++i)
		m_pEnemys[i]->Update(fTimeElapsed);

	
	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Update(fTimeElapsed);
	}

	m_pMainCharacter->Update(fTimeElapsed);

	for (int i = 0; i < m_pBillBoardObjects.size(); ++i)
	{
		m_pBillBoardObjects[i]->Animate(fTimeElapsed);
	}

	m_pSimulaterCamera->Update(XMFLOAT3{ 0.0f, 0.0f, 0.0f }, fTimeElapsed);

	CameraUpdateParams camera_update_params;
	camera_update_params.pCamera = m_pSimulaterCamera.get();
	camera_update_params.pPlayer = m_pMainCharacter.get();
	camera_update_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<CameraUpdateParams>(MessageType::UPDATE_CAMERA, &camera_update_params, m_pMainCharacter->m_pStateMachine->GetCurrentState());
}

void CSimulatorScene::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	m_pPostProcessShader->OnPrepareRenderTarget(pd3dCommandList, 1, pd3dRtvCPUHandles, d3dDepthStencilBufferDSVCPUHandle);
	
}
void CSimulatorScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{     
	m_pTextureManager->SetTextureDescriptorHeap(pd3dCommandList);
	m_pTextureManager->UpdateShaderVariables(pd3dCommandList);

	m_pSimulaterCamera->RegenerateViewMatrix();
	m_pSimulaterCamera->OnPrepareRender(pd3dCommandList);

	m_pLight->Render(pd3dCommandList);

	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);

	CModelShader::GetInst()->Render(pd3dCommandList, 1);

	m_pMainCharacter->Animate(0.0f);
	m_pMainCharacter->Render(pd3dCommandList, true);

	static float trailUpdateT = 0.0f;
	trailUpdateT += fTimeElapsed;
	if (trailUpdateT > 0.008f) {
		for (int i = 0; i < m_pSwordTrailObjects.size(); ++i) {
			CSwordTrailObject* trailObj = dynamic_cast<CSwordTrailObject*>(m_pSwordTrailObjects[i].get());
			if (trailObj->m_bIsUpdateTrailVariables)
				trailObj->SetNextControllPoint(&(m_pMainCharacter->GetTrailControllPoint(0)), &((CPlayer*)m_pMainCharacter.get())->GetTrailControllPoint(1));
			else
				trailObj->SetNextControllPoint(nullptr, nullptr);
		}

		trailUpdateT = 0.0f;
	}

	for (int i = 0; i < m_pEnemys.size(); ++i)
	{
		m_pEnemys[i]->Animate(0.0f);
		m_pEnemys[i]->Render(pd3dCommandList, true);
	}

#define PostProcessing
#ifdef PostProcessing
	m_pPostProcessShader->Render(pd3dCommandList, pCamera);
#endif // PostProcessing
	
	m_pBillBoardObjectShader->Render(pd3dCommandList, 0);

	for (int i = 0; i < m_pBillBoardObjects.size(); ++i)
	{
		(static_cast<CBillBoardObject*>(m_pBillBoardObjects[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		m_pBillBoardObjects[i]->Render(pd3dCommandList, false);
	}

	/*for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		(static_cast<CTerrainSpriteObject*>(m_pTerrainSpriteObject[i].get()))->Animate(m_pTerrain.get(), fTimeElapsed);
		m_pTerrainSpriteObject[i]->Render(pd3dCommandList, true);
	}*/

	//for (int i = 0; i < m_pUpDownParticleObjects.size(); ++i)
	//{
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Update(fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	//}

	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}

	m_pSwordTrailShader->Render(pd3dCommandList, pCamera, 0);
	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Render(pd3dCommandList, true);
	}

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
void CSimulatorScene::OnPostRender()
{
	m_pSimulaterCamera->OnPostRender();

	for (int i = 0; i < m_pParticleObjects.size(); i++)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->OnPostRender();
	}

	//for (int i = 0; i < m_pUpDownParticleObjects.size(); i++)
	//{
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->OnPostRender();
	//}
}

void CSimulatorScene::ResetMonster()
{
	XMFLOAT3 offset{ 86.4804 , 0.0f, -183.7856 };
	XMFLOAT3 xmf3LookAt{ m_pMainCharacter->GetPosition().x, m_pEnemys[0]->GetPosition().y, m_pMainCharacter->GetPosition().z};
	
	m_pEnemys[0]->SetPosition(XMFLOAT3(50 + offset.x, 100, 50 + offset.z));
	m_pEnemys[0]->SetScale(4.0f, 4.0f, 4.0f);
	m_pEnemys[0]->SetLookAt(xmf3LookAt);
	(dynamic_cast<CMonster*>(m_pEnemys[0].get()))->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
}

void CSimulatorScene::SetPlayerAnimationSet(int nSet)
{
	switch (nSet)
	{
	case 0:
		m_pMainCharacter->m_pStateMachine->ChangeState(Atk1_Player::GetInst());
		m_pMainCharacter->m_iAttackId += 1;
		break;
	case 1:
		m_pMainCharacter->m_pStateMachine->ChangeState(Atk2_Player::GetInst());
		m_pMainCharacter->m_iAttackId += 1;
		break;
	case 2:
		m_pMainCharacter->m_pStateMachine->ChangeState(Atk3_Player::GetInst());
		m_pMainCharacter->m_iAttackId += 1;
		break;
	default:
		break;
	}

	return;

	m_pMainCharacter->m_pSkinnedAnimationController->SetTrackAnimationSet(0, nSet);
	m_pMainCharacter->m_pSkinnedAnimationController->m_fTime = 0.0f;
	m_pMainCharacter->Animate(0.0f);
}

void CSimulatorScene::HandleCollision(const CollideParams& params)
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
		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_PARTICLE, &particle_comp_params, m_pMainCharacter->m_pStateMachine->GetCurrentState());
	}

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
	
	it = std::find_if(m_pBillBoardObjects.begin(), m_pBillBoardObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
		if (!((CMultiSpriteObject*)pBillBoardObject.get())->GetEnable())
			return true;
		return false;
		});

	if (it != m_pBillBoardObjects.end())
	{
		ImpactCompParams impact_comp_params;
		impact_comp_params.pObject = (*it).get();
		impact_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<ImpactCompParams>(MessageType::UPDATE_BILLBOARD, &impact_comp_params, m_pMainCharacter->m_pStateMachine->GetCurrentState());
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
		CMessageDispatcher::GetInst()->Dispatch_Message<TerrainSpriteCompParams>(MessageType::UPDATE_SPRITE, &AttackSprite_comp_params, nullptr);
	}
}


