#include "Texture.h"
#include "Object.h"
#include "..\Shader\Shader.h"

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers.resize(m_nTextures); // 업로드 버퍼 생성
		m_ppd3dTextures.resize(m_nTextures); // 텍스쳐 리소스 생성

		m_ppstrTextureNames.resize(m_nTextures); // 각 텍스쳐의 이름을 저장하는 배열 생성
		for (int i = 0; i < m_nTextures; i++) {
			m_ppstrTextureNames[i].resize(64);
			m_ppstrTextureNames[i][0] = '\0';
		}

		m_pd3dSrvGpuDescriptorHandles.resize(m_nTextures); // 
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes.resize(m_nTextures);
		m_pdxgiBufferFormats.resize(m_nTextures);
		m_pnBufferElements.resize(m_nTextures);
		for (int i = 0; i < m_nTextures; i++) m_pnBufferElements[i] = 0;
		m_pnBufferStrides.resize(m_nTextures);
		for (int i = 0; i < m_nTextures; i++) m_pnBufferStrides[i] = 0;
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices.resize(nRootParameters);
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;
}

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters, int nGraphicsSrvGpuHandles, int nComputeUavRootParameters, int nComputeSrvRootParameters, int nComputeUavGpuHandles, int nComputeSrvGpuHandles)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers.resize(m_nTextures); // 업로드 버퍼 생성
		m_ppd3dTextures.resize(m_nTextures); // 텍스쳐 리소스 생성

		m_ppstrTextureNames.resize(m_nTextures); // 각 텍스쳐의 이름을 저장하는 배열 생성
		for (int i = 0; i < m_nTextures; i++) {
			m_ppstrTextureNames[i].resize(64);
			m_ppstrTextureNames[i][0] = '\0';
		}

		m_pd3dSrvGpuDescriptorHandles.resize(nGraphicsSrvGpuHandles); // 
		for (int i = 0; i < nGraphicsSrvGpuHandles; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pd3dComputeUavGpuDescriptorHandles.resize(nComputeUavGpuHandles);
		for (int i = 0; i < nComputeUavGpuHandles; i++) m_pd3dComputeUavGpuDescriptorHandles[i].ptr = NULL;

		m_pd3dComputeSrvGpuDescriptorHandles.resize(nComputeSrvGpuHandles);
		for (int i = 0; i < nComputeSrvGpuHandles; i++) m_pd3dComputeSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes.resize(m_nTextures);
		m_pdxgiBufferFormats.resize(m_nTextures);
		m_pnBufferElements.resize(m_nTextures);
		for (int i = 0; i < m_nTextures; i++) m_pnBufferElements[i] = 0;
		m_pnBufferStrides.resize(m_nTextures);
		for (int i = 0; i < m_nTextures; i++) m_pnBufferStrides[i] = 0;
	}

	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices.resize(nRootParameters);
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;


	m_nComputeUavRootParameters = nComputeUavRootParameters;
	if (m_nComputeUavRootParameters > 0)
	{
		m_pnComputeUavRootParameterIndices.resize(m_nComputeUavRootParameters);
		for (int i = 0; i < m_nComputeUavRootParameters; i++) m_pnComputeUavRootParameterIndices[i] = -1;
		m_pnComputeUavRootParameteDescriptors.resize(m_nComputeUavRootParameters);
		for (int i = 0; i < m_nComputeUavRootParameters; i++) m_pnComputeUavRootParameteDescriptors[i] = -1;
		m_pd3dComputeUavRootParameterGpuDescriptorHandles.resize(m_nComputeUavRootParameters);
		for (int i = 0; i < m_nComputeUavRootParameters; i++) m_pd3dComputeUavRootParameterGpuDescriptorHandles[i].ptr = NULL;
	}

	m_nComputeSrvRootParameters = nComputeSrvRootParameters;
	if (m_nComputeSrvRootParameters > 0)
	{
		m_pnComputeSrvRootParameterIndices.resize(m_nComputeSrvRootParameters);
		for (int i = 0; i < m_nComputeSrvRootParameters; i++) m_pnComputeSrvRootParameterIndices[i] = -1;
		m_pnComputeSrvRootParameterDescriptors.resize(m_nComputeSrvRootParameters);
		for (int i = 0; i < m_nComputeSrvRootParameters; i++) m_pnComputeSrvRootParameterDescriptors[i] = -1;
		m_pd3dComputeSrvRootParameterGpuDescriptorHandles.resize(m_nComputeSrvRootParameters);
		for (int i = 0; i < m_nComputeSrvRootParameters; i++) m_pd3dComputeSrvRootParameterGpuDescriptorHandles[i].ptr = NULL;
	}
}

CTexture::~CTexture()
{
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;

}
void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{

		if (m_pd3dSrvGpuDescriptorHandles[0].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}
ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth,
	UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags,
	D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex].Get());
}
void CTexture::SetComputeSrvRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nSrvDescriptors)
{
	m_pnComputeSrvRootParameterIndices[nIndex] = nRootParameterIndex;
	m_pd3dComputeSrvRootParameterGpuDescriptorHandles[nIndex] = m_pd3dComputeSrvGpuDescriptorHandles[nGpuHandleIndex];
	m_pnComputeSrvRootParameterDescriptors[nIndex] = nSrvDescriptors;
}
void CTexture::SetComputeUavRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nUavDescriptors)
{
	m_pnComputeUavRootParameterIndices[nIndex] = nRootParameterIndex;
	m_pd3dComputeUavRootParameterGpuDescriptorHandles[nIndex] = m_pd3dComputeUavGpuDescriptorHandles[nGpuHandleIndex];
	m_pnComputeUavRootParameteDescriptors[nIndex] = nUavDescriptors;
}
void CTexture::SetComputeSrvGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dComputeSrvGpuDescriptorHandles[nHandleIndex] = d3dSrvGpuDescriptorHandle;
}
void CTexture::SetComputeUavGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle)
{
	m_pd3dComputeUavGpuDescriptorHandles[nHandleIndex] = d3dUavGpuDescriptorHandle;
}
void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::ReleaseShaderVariables()
{
}
void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers.data())
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i].Reset();
		m_ppd3dTextureUploadBuffers.clear();
	}
}

void CTexture::SetRowColumn(int iRow, int iColumn)
{
	m_iRow = iRow;
	m_iColumn = iColumn;
}

int CTexture::GetRow()
{
	return m_iRow;
}

int CTexture::GetColumn()
{
	return m_iColumn;
}



void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
	m_ppstrTextureNames[nIndex] = pszFileName;
}
void CTexture::UpdateComputeSrvShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex)
{
	if ((m_pnComputeSrvRootParameterIndices[nIndex] != -1) && (m_pd3dComputeSrvRootParameterGpuDescriptorHandles[nIndex].ptr != NULL)) 
		pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeSrvRootParameterIndices[nIndex], m_pd3dComputeSrvRootParameterGpuDescriptorHandles[nIndex]);
}
void CTexture::CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_pnBufferStrides[nIndex] = nStride;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, d3dHeapType, d3dResourceStates, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex].Get());
}
void CTexture::UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nComputeSrvRootParameters; i++)
	{
		if ((m_pnComputeSrvRootParameterIndices[i] != -1) && (m_pd3dComputeSrvRootParameterGpuDescriptorHandles[i].ptr != NULL)) pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeSrvRootParameterIndices[i], m_pd3dComputeSrvRootParameterGpuDescriptorHandles[i]);
	}

	for (int i = 0; i < m_nComputeUavRootParameters; i++)
	{
		if ((m_pnComputeUavRootParameterIndices[i] != -1) && (m_pd3dComputeUavRootParameterGpuDescriptorHandles[i].ptr != NULL)) pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeUavRootParameterIndices[i], m_pd3dComputeUavRootParameterGpuDescriptorHandles[i]);
	}
}
ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, 1, 0, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex].Get());
}
int CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Object/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 16, 64 - 16, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 16 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 16 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, const_cast<wchar_t*>(m_ppstrTextureNames[nIndex].data()), 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', m_ppstrTextureNames[nIndex].c_str());
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex].data(), RESOURCE_TEXTURE2D, nIndex);
			pShader->CreateShaderResourceView(pd3dDevice, this, nIndex);
		}
		else
		{
			if (pParent)
			{
				CGameObject* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->GetParent()) break;
					pRootGameObject = pRootGameObject->GetParent();
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(const_cast<wchar_t*>(m_ppstrTextureNames[nIndex].data()), &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
					m_pnRootParameterIndices[nIndex] = nParameterIndex;
				}
			}
		}
	}
	return(bLoaded);
}

void CTexture::UpdateComputeUavShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex)
{
	if ((m_pnComputeUavRootParameterIndices[nIndex] != -1) && (m_pd3dComputeUavRootParameterGpuDescriptorHandles[nIndex].ptr != NULL)) 
		pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeUavRootParameterIndices[nIndex], m_pd3dComputeUavRootParameterGpuDescriptorHandles[nIndex]);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	D3D12_RESOURCE_DESC d3dResourceDesc = m_ppd3dTextures[nIndex]->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = m_pnResourceTypes[nIndex];
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE1D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
		d3dShaderResourceViewDesc.Texture1D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		//if (d3dResourceDesc.Format == DXGI_FORMAT_D32_FLOAT) d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	case RESOURCE_STRUCTURED_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = m_pnBufferStrides[nIndex];
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_UNORDERED_ACCESS_VIEW_DESC CTexture::GetUnorderedAccessViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUnorderedAccessViewDesc;

	int nTextureType = GetTextureType();
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dUnorderedAccessViewDesc.Format = d3dResourceDesc.Format;
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		d3dUnorderedAccessViewDesc.Texture2D.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dUnorderedAccessViewDesc.Format = d3dResourceDesc.Format;
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		d3dUnorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		d3dUnorderedAccessViewDesc.Texture2DArray.PlaneSlice = 0;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dUnorderedAccessViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		d3dUnorderedAccessViewDesc.Buffer.FirstElement = 0;
		d3dUnorderedAccessViewDesc.Buffer.NumElements = 0;
		d3dUnorderedAccessViewDesc.Buffer.StructureByteStride = 0;
		d3dUnorderedAccessViewDesc.Buffer.CounterOffsetInBytes = 0;
		d3dUnorderedAccessViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		break;
	}
	return(d3dUnorderedAccessViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMaterial::CMaterial()
{
}
CMaterial::~CMaterial()
{
}
void CMaterial::SetShader(std::shared_ptr<CShader> pShader)
{
	m_pShader = pShader;
}
void CMaterial::SetTexture(std::shared_ptr<CTexture> pTexture)
{
	m_pTexture = pTexture;
}
void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}
void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_nType, 32);
}
void CMaterial::ReleaseShaderVariables()
{
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}