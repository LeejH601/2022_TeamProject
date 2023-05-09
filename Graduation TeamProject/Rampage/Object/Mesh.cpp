#include "Mesh.h"
#include "Object.h"
#include "..\Shader\BoundingBoxShader.h"

// Mesh 생성자
CMesh::~CMesh()
{
}
void CMesh::ReleaseUploadBuffers()
{
	// UploadBuffer를 Default힙에 내용을 전달한 후 불려지는 함수
	// 오브젝트를 생성한 뒤 필요없어진 UploadBuffer들을 Release() 하는 함수
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer.Reset();
	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer.Reset();
	if (m_ppd3dIndexUploadBuffers.data())
	{
		m_ppd3dIndexUploadBuffers.clear();
		std::vector<ComPtr<ID3D12Resource>>().swap(m_ppd3dIndexUploadBuffers);
	}
};
void CMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 기본도형 위상구조를 삼각형리스트로 설정
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// 정점 버퍼 뷰를 파이프라인의 입력 슬롯에 binding 한다.
	// 첫번째 인자는 입력 슬롯, 두번째는 정점 뷰의 개수, 세번째는 정점 버퍼 뷰의 첫 원소를 가르키는 포인터이다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_pd3dVertexBufferViews.size(), m_pd3dVertexBufferViews.data());
}
void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	// 메쉬를 렌더링하는 함수이다.
	if ((m_nSubMeshes > 0) && (nSubset < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[nSubset]);
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubset], 1, 0, 0, 0);
	}
	else
	{

		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_ppd3dIndexBuffers.empty())
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[0]);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
void CTexturedModelingMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 기본도형 위상구조를 삼각형리스트로 설정
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// 정점 버퍼 뷰를 파이프라인의 입력 슬롯에 binding 한다.
	// 첫번째 인자는 입력 슬롯, 두번째는 정점 뷰의 개수, 세번째는 정점 버퍼 뷰의 첫 원소를 가르키는 포인터이다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_pd3dVertexBufferViews.size(), m_pd3dVertexBufferViews.data());
}
void CTexturedModelingMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	// 메쉬를 렌더링하는 함수이다.
	if ((m_nSubMeshes > 0) && (nSubset < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[nSubset]);
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubset], 1, 0, 0, 0);
	}
	else
	{

		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
void CTexturedModelingMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrMeshName[nStrLength] = '\0';

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions.resize(nPositions);
				nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(XMFLOAT3), nPositions, pInFile);
				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
				m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_pd3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_pd3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				std::vector<XMFLOAT4> m_pxmf4Colors;
				m_pxmf4Colors.resize(nColors);
				nReads = (UINT)::fread(m_pxmf4Colors.data(), sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dTextureCoord0BufferView;
				m_pd3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_pd3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_pd3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_pd3dTextureCoord0BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;

				m_pxmf2TextureCoords1.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dTextureCoord1BufferView;
				m_pd3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_pd3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_pd3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				//m_pd3dVertexBufferViews.push_back(m_pd3dTextureCoord1BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals.resize(nNormals);
				nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dNormalBufferView;
				m_pd3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_pd3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_pd3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_pd3dNormalBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents.resize(nTangents);
				nReads = (UINT)::fread(m_pxmf3Tangents.data(), sizeof(XMFLOAT3), nTangents, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents.resize(nBiTangents);
				nReads = (UINT)::fread(m_pxmf3BiTangents.data(), sizeof(XMFLOAT3), nBiTangents, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				// 서브 메쉬 인덱스 개수
				m_pnSubSetIndices.resize(m_nSubMeshes);
				m_ppnSubSetIndices.resize(m_nSubMeshes);


				// 서브 메쉬 인덱스 버퍼
				m_ppd3dIndexBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 업로드 버퍼
				m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 버퍼 뷰
				m_pd3dIndexBufferViews.resize(m_nSubMeshes);

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							// 서브 메쉬들의 인덱스 들
							m_ppnSubSetIndices[i].resize(m_pnSubSetIndices[i]);
							nReads = (UINT)::fread(m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], 1, pInFile);

							m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

							m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}
void CSkinnedMesh::PrepareSkinning(CGameObject* pModelRootObject)
{
	for (int j = 0; j < m_nSkinningBones; j++)
	{
		m_ppSkinningBoneFrameCaches[j] = pModelRootObject->FindFrame(m_ppstrSkinningBoneNames[j].data());
	}
}
void CSkinnedMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 기본도형 위상구조를 삼각형리스트로 설정
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// 정점 버퍼 뷰를 파이프라인의 입력 슬롯에 binding 한다.
	// 첫번째 인자는 입력 슬롯, 두번째는 정점 뷰의 개수, 세번째는 정점 버퍼 뷰의 첫 원소를 가르키는 포인터이다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_pd3dVertexBufferViews.size(), m_pd3dVertexBufferViews.data());
}
void CSkinnedMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	UpdateShaderVariables(pd3dCommandList);

	// 메쉬를 렌더링하는 함수이다.
	if ((m_nSubMeshes > 0) && (nSubset < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[nSubset]);
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubset], 1, 0, 0, 0);
	}
	else
	{

		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
void CSkinnedMesh::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBindPoseBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
	}

	if (m_pd3dcbSkinningBoneTransforms)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pd3dcbSkinningBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms

		for (int j = 0; j < m_nSkinningBones; j++)
		{
			XMStoreFloat4x4(&m_pcbxmf4x4MappedSkinningBoneTransforms[j], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));
		}
	}
}
void CSkinnedMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions.resize(nPositions);
				nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(XMFLOAT3), nPositions, pInFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				if (m_pd3dVertexBufferViews.size() < 2)
				{
					m_d3dBoneIndexBufferView.BufferLocation = NULL;
					m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMINT4);
					m_d3dBoneIndexBufferView.SizeInBytes = 0;
					m_pd3dVertexBufferViews.push_back(m_d3dBoneIndexBufferView);

					m_d3dBoneWeightBufferView.BufferLocation = NULL;
					m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
					m_d3dBoneWeightBufferView.SizeInBytes = 0;
					m_pd3dVertexBufferViews.push_back(m_d3dBoneWeightBufferView);
				}

				D3D12_VERTEX_BUFFER_VIEW m_d3dPositionBufferView;
				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dPositionBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				std::vector<XMFLOAT4> m_pxmf4Colors;
				m_pxmf4Colors.resize(nColors);
				nReads = (UINT)::fread(m_pxmf4Colors.data(), sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_d3dTextureCoord0BufferView;
				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dTextureCoord0BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_pxmf2TextureCoords1.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_d3dTextureCoord1BufferView;
				m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				//m_pd3dVertexBufferViews.push_back(m_d3dTextureCoord1BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals.resize(nNormals);
				nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_d3dNormalBufferView;
				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dNormalBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents.resize(nTangents);
				nReads = (UINT)::fread(m_pxmf3Tangents.data(), sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_d3dTangentBufferView;
				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dTangentBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents.resize(nBiTangents);
				nReads = (UINT)::fread(m_pxmf3BiTangents.data(), sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_d3dBiTangentBufferView;
				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dBiTangentBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices.resize(m_nSubMeshes);
				m_ppnSubSetIndices.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 버퍼
				m_ppd3dIndexBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 업로드 버퍼
				m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 버퍼 뷰
				m_pd3dIndexBufferViews.resize(m_nSubMeshes);

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile); //i
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							// 서브 메쉬들의 인덱스 들
							m_ppnSubSetIndices[i].resize(m_pnSubSetIndices[i]);
							nReads = (UINT)::fread(m_ppnSubSetIndices[i].data(), sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

							m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}
void CSkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			m_nBonesPerVertex = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames.resize(m_nSkinningBones);
				m_ppSkinningBoneFrameCaches.resize(m_nSkinningBones);
				for (int i = 0; i < m_nSkinningBones; i++)
				{
					::ReadStringFromFile(pInFile, const_cast<char*>(m_ppstrSkinningBoneNames[i].data()));
					m_ppSkinningBoneFrameCaches[i] = NULL;
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_pxmf4x4BindPoseBoneOffsets.resize(m_nSkinningBones);
				nReads = (UINT)::fread(m_pxmf4x4BindPoseBoneOffsets.data(), sizeof(XMFLOAT4X4), m_nSkinningBones, pInFile);

				UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
				m_pd3dcbBindPoseBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_pd3dcbBindPoseBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4MappedBindPoseBoneOffsets);

				for (int i = 0; i < m_nSkinningBones; i++)
				{
					XMStoreFloat4x4(&m_pcbxmf4x4MappedBindPoseBoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmn4BoneIndices.resize(m_nVertices);

				nReads = (UINT)::fread(m_pxmn4BoneIndices.data(), sizeof(XMINT4), m_nVertices, pInFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmn4BoneIndices.data(), sizeof(XMINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMINT4) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dBoneIndexBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmf4BoneWeights.resize(m_nVertices);

				nReads = (UINT)::fread(m_pxmf4BoneWeights.data(), sizeof(XMFLOAT4), m_nVertices, pInFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4BoneWeights.data(), sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_d3dBoneWeightBufferView);
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}
CRawFormatImage::CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	BYTE* pRawImagePixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pRawImagePixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	if (bFlipY)
	{
		m_pRawImagePixels = new BYTE[m_nWidth * m_nLength];
		for (int z = 0; z < m_nLength; z++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				m_pRawImagePixels[x + ((m_nLength - 1 - z) * m_nWidth)] = pRawImagePixels[x + (z * m_nWidth)];
			}
		}

		if (pRawImagePixels) delete[] pRawImagePixels;
	}
	else
	{
		m_pRawImagePixels = pRawImagePixels;
	}
}
CRawFormatImage::~CRawFormatImage()
{
	if (m_pRawImagePixels) delete[] m_pRawImagePixels;
	m_pRawImagePixels = NULL;
}
CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale) : CRawFormatImage(pFileName, nWidth, nLength, true)
{
	m_xmf3Scale = xmf3Scale;
}
CHeightMapImage::~CHeightMapImage()
{
}
XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pRawImagePixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pRawImagePixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pRawImagePixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

TangentEdges CHeightMapImage::GetHeightMapTangent(int x, int z)
{
	//if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pRawImagePixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pRawImagePixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pRawImagePixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	TangentEdges edges = { xmf3Edge1, xmf3Edge2 };
	return edges;
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pRawImagePixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pRawImagePixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pRawImagePixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pRawImagePixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}
CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
{
	m_nVertices = nWidth * nLength;
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetRawImageWidth();
	int czHeightMap = pHeightMapImage->GetRawImageLength();


	m_pxmf3Positions.resize(m_nVertices);
	m_pxmf4Colors.resize(m_nVertices);
	m_pxmf2TextureCoords0.resize(m_nVertices);
	m_pxmf2TextureCoords1.resize(m_nVertices);

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			m_pxmf4Colors[i] = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			m_pxmf2TextureCoords0[i] = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			m_pxmf2TextureCoords1[i] = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_nVertexBufferViews = 4;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);
	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);;
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dColorBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4Colors.data(), sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dColorUploadBuffer);
	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dColorBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT4);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;

	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);
	m_pd3dVertexBufferViews[2].BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);
	m_pd3dVertexBufferViews[3].BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[3].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[3].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_nSubMeshes = 1;
	m_pnSubSetIndices.resize(m_nSubMeshes);
	m_ppnSubSetIndices.resize(m_nSubMeshes);


	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);


	m_pnSubSetIndices[0] = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	m_ppnSubSetIndices[0].resize(m_pnSubSetIndices[0]);

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0].data(), sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];
}
CHeightMapGridMesh::~CHeightMapGridMesh()
{
	if (!m_pxmf4Colors.empty()) m_pxmf4Colors.clear();
	if (!m_pxmf2TextureCoords0.empty()) m_pxmf2TextureCoords0.clear();
	if (!m_pxmf2TextureCoords1.empty()) m_pxmf2TextureCoords1.clear();
}
void CHeightMapGridMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dColorUploadBuffer) m_pd3dColorUploadBuffer->Release();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();

	if (m_pd3dTextureCoord1UploadBuffer) m_pd3dTextureCoord1UploadBuffer->Release();

}
float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext, bool SampleByImage)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetRawImagePixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetRawImageWidth();
	float fHeight;
	if (SampleByImage)
		fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	else
		fHeight = m_pxmf3Positions[x + (z * nWidth)].y;
	return(fHeight);
}
XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

CSplatGridMesh::CSplatGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
{
	m_nVertices = nWidth * nLength;
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetRawImageWidth();
	int czHeightMap = pHeightMapImage->GetRawImageLength();


	m_pxmf3Positions.resize(m_nVertices);
	m_pxmf4Colors.resize(m_nVertices);
	m_pxmf2TextureCoords0.resize(m_nVertices);
	m_pxmf2TextureCoords1.resize(m_nVertices);

	auto clamp = [&](float value) {
		return value < 0 ? 0 : (value > m_nLength - 1 ? m_nLength - 1 : value);
	};

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//fHeight = OnGetHeight(x, z, pContext);
			float _h01 = OnGetHeight(x, clamp(z - 2), pContext, true);
			float _h10 = OnGetHeight(clamp(x - 2), z, pContext, true);
			float _h11 = OnGetHeight(clamp(x - 2), clamp(z - 2), pContext, true);
			float _h_01 = OnGetHeight(clamp(x - 2), clamp(z + 2), pContext, true);
			float _h0_1 = OnGetHeight(clamp(x + 2), clamp(z - 2), pContext, true);
			float h00 = OnGetHeight(x, z, pContext, true);
			float h01 = OnGetHeight(x, clamp(z + 2), pContext, true);
			float h10 = OnGetHeight(clamp(x + 2), z, pContext, true);
			float h11 = OnGetHeight(clamp(x + 2), clamp(z + 2), pContext, true);
			float fx = 0.5f, fz = 0.5f;
			float hRB = (1 - fx) * ((1 - fz) * h00 + fz * h01) + fx * ((1 - fz) * h10 + fz * h11);
			float hRT = (1 - fx) * ((1 - fz) * _h01 + fz * h00) + fx * ((1 - fz) * _h0_1 + fz * h10);
			float hLB = (1 - fx) * ((1 - fz) * _h10 + fz * _h_01) + fx * ((1 - fz) * h00 + fz * h01);
			float hLT = (1 - fx) * ((1 - fz) * _h11 + fz * _h10) + fx * ((1 - fz) * _h01 + fz * h00);
			fHeight = (1 - fx) * ((1 - fz) * hLT + fz * hLB) + fx * ((1 - fz) * hRT + fz * hRB);
			m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			m_pxmf4Colors[i] = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			m_pxmf2TextureCoords0[i] = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 35.0f), float(z) / float(m_xmf3Scale.z * 35.0f));
			m_pxmf2TextureCoords1[i] = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}


	//std::vector<XMFLOAT3> pPositions = m_pxmf3Positions;
	//for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	//{
	//	for (int x = xStart; x < (xStart + nWidth); x++, i++)
	//	{
	//		//x + (z * nWidth)
	//		//fHeight = OnGetHeight(x, z, pContext);
	//		float _h01 = pPositions[x + clamp(z - 2) * nWidth].y;
	//		float _h10 = pPositions[clamp(x - 2) + z * nWidth].y;
	//		float _h11 = pPositions[clamp(x - 2) + clamp(z - 2) * nWidth].y;
	//		float _h_01 = pPositions[clamp(x - 2) + clamp(z + 2) * nWidth].y;
	//		float _h0_1 = pPositions[clamp(x + 2) + clamp(z - 2) * nWidth].y;
	//		float h00 = pPositions[x + z * nWidth].y;
	//		float h01 = pPositions[x + clamp(z + 2) * nWidth].y;
	//		float h10 = pPositions[clamp(x + 2) + z * nWidth].y;
	//		float h11 = pPositions[clamp(x + 2) + clamp(z + 2) * nWidth].y;
	//		float fx = 0.5f, fz = 0.5f;
	//		float hRB = (1 - fx) * ((1 - fz) * h00 + fz * h01) + fx * ((1 - fz) * h10 + fz * h11);
	//		float hRT = (1 - fx) * ((1 - fz) * _h01 + fz * h00) + fx * ((1 - fz) * _h0_1 + fz * h10);
	//		float hLB = (1 - fx) * ((1 - fz) * _h10 + fz * _h_01) + fx * ((1 - fz) * h00 + fz * h01);
	//		float hLT = (1 - fx) * ((1 - fz) * _h11 + fz * _h10) + fx * ((1 - fz) * _h01 + fz * h00);
	//		fHeight = (1 - fx) * ((1 - fz) * hLT + fz * hLB) + fx * ((1 - fz) * hRT + fz * hRB);
	//		m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
	//	}
	//}

	/*for (int i = 0, z = zStart + 1; z < (zStart + nLength - 2); z++)
	{
		for (int x = xStart + 1; x < (xStart + nWidth - 2); x++, i++)
		{
			XMFLOAT3 p0 = pPositions[x + (z * cxHeightMap) - 1];
			XMFLOAT3 p1 = pPositions[x + (z * cxHeightMap)];
			XMFLOAT3 p2 = pPositions[x + (z * cxHeightMap) + 1];
			XMFLOAT3 p3 = pPositions[x + (z * cxHeightMap) + 2];

			XMVectorCatmullRom(XMLoadFloat3(&p0), XMLoadFloat3(&p1), XMLoadFloat3(&p2), XMLoadFloat3(&p3), );
		}
	}*/

	m_pxmf3Normals.resize(m_nVertices);
	m_pxmf3Tangents.resize(m_nVertices);

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			m_pxmf3Normals[i] = pHeightMapImage->GetHeightMapNormal(x, z);

			TangentEdges edges = pHeightMapImage->GetHeightMapTangent(x, z);

			int nHeightMapIndex = x + (z * m_nWidth);
			int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
			int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

			XMFLOAT2 uv0 = m_pxmf2TextureCoords0[nHeightMapIndex];
			XMFLOAT2 uv1 = m_pxmf2TextureCoords0[nHeightMapIndex + zHeightMapAdd];
			XMFLOAT2 uv2 = m_pxmf2TextureCoords0[nHeightMapIndex + xHeightMapAdd];

			XMFLOAT4X4 uvMatrix = Matrix4x4::Identity();
			uvMatrix._11 = (uv1.x - uv0.x);
			uvMatrix._12 = (uv1.y - uv0.y);
			uvMatrix._21 = (uv2.x - uv0.x);
			uvMatrix._22 = (uv2.y - uv0.y);

			uvMatrix = Matrix4x4::Inverse(uvMatrix);

			XMFLOAT4X4 edgeMatrix = Matrix4x4::Identity();
			edgeMatrix._11 = edges.e0.x;
			edgeMatrix._12 = edges.e0.y;
			edgeMatrix._13 = edges.e0.z;
			edgeMatrix._21 = edges.e1.x;
			edgeMatrix._22 = edges.e1.y;
			edgeMatrix._23 = edges.e1.z;

			XMFLOAT4X4 result = Matrix4x4::Multiply(uvMatrix, edgeMatrix);

			m_pxmf3Tangents[i] = Vector3::Normalize(XMFLOAT3(result._11, result._12, result._13));
		}
	}

	m_nVertexBufferViews = 6;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);
	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);;
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dColorBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4Colors.data(), sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dColorUploadBuffer);
	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dColorBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT4);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;

	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);
	m_pd3dVertexBufferViews[2].BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);
	m_pd3dVertexBufferViews[3].BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[3].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[3].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	m_pd3dVertexBufferViews[4].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[4].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[4].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);
	m_pd3dVertexBufferViews[5].BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[5].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[5].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_nSubMeshes = 1;
	m_pnSubSetIndices.resize(m_nSubMeshes);
	m_ppnSubSetIndices.resize(m_nSubMeshes);


	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);


	m_pnSubSetIndices[0] = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	m_ppnSubSetIndices[0].resize(m_pnSubSetIndices[0]);

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0].data(), sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];
}

CSplatGridMesh::~CSplatGridMesh()
{
}
//-------------------------------------------------------------------
CBoundingBoxMesh::CBoundingBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3AABBCenter, XMFLOAT3 xmf3AABBExtents)
{
	//BoundingBox는 직육면체이다. 
	m_nVertices = 8;
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = xmf3AABBExtents.x, fy = xmf3AABBExtents.y, fz = xmf3AABBExtents.z;

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(-fx, +fy, -fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(+fx, +fy, -fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(+fx, +fy, +fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(-fx, +fy, +fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(-fx, -fy, -fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(+fx, -fy, -fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(+fx, -fy, +fz)));
	m_pxmf3Positions.push_back(Vector3::Add(xmf3AABBCenter, XMFLOAT3(-fx, -fy, +fz)));

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_nVertexBufferViews = 1;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_nSubMeshes = 1;
	m_nIndices = 36;
	m_pnSubSetIndices.resize(m_nSubMeshes);
	m_ppnSubSetIndices.resize(m_nSubMeshes);

	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);

	m_pnSubSetIndices[0] = m_nIndices;

	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(3); m_ppnSubSetIndices[0].push_back(1); m_ppnSubSetIndices[0].push_back(0);
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(2); m_ppnSubSetIndices[0].push_back(1); m_ppnSubSetIndices[0].push_back(3);
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(0); m_ppnSubSetIndices[0].push_back(5); m_ppnSubSetIndices[0].push_back(4);
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(1); m_ppnSubSetIndices[0].push_back(5); m_ppnSubSetIndices[0].push_back(0);
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(3); m_ppnSubSetIndices[0].push_back(4); m_ppnSubSetIndices[0].push_back(7);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(0); m_ppnSubSetIndices[0].push_back(4); m_ppnSubSetIndices[0].push_back(3);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(1); m_ppnSubSetIndices[0].push_back(6); m_ppnSubSetIndices[0].push_back(5);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(2); m_ppnSubSetIndices[0].push_back(6); m_ppnSubSetIndices[0].push_back(1);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(2); m_ppnSubSetIndices[0].push_back(7); m_ppnSubSetIndices[0].push_back(6);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(3); m_ppnSubSetIndices[0].push_back(7); m_ppnSubSetIndices[0].push_back(2);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	m_ppnSubSetIndices[0].push_back(6); m_ppnSubSetIndices[0].push_back(4); m_ppnSubSetIndices[0].push_back(5);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	m_ppnSubSetIndices[0].push_back(7); m_ppnSubSetIndices[0].push_back(4); m_ppnSubSetIndices[0].push_back(6);

	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0].data(), sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];
}
CBoundingBoxMesh::~CBoundingBoxMesh()
{
}

CTexturedRectMesh::CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition) : CMesh()
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmfTexturedVertexs.reserve(m_nVertices);

	float fx = (fWidth * 0.5f) + fxPosition, fy = (fHeight * 0.5f) + fyPosition, fz = (fDepth * 0.5f) + fzPosition;

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f)));
		}
		else
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f)));
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, -fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, +fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, -fz), XMFLOAT2(1.0f, 0.0f)));
		}
		else
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f)));
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, fz), XMFLOAT2(1.0f, 0.0f)));
		}
		else
		{
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, -fy, fz), XMFLOAT2(0.0f, 1.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(fx, fy, fz), XMFLOAT2(0.0f, 0.0f)));
			m_pxmfTexturedVertexs.push_back(CTexturedVertex(XMFLOAT3(-fx, fy, fz), XMFLOAT2(1.0f, 0.0f)));
		}
	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmfTexturedVertexs.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dPositionBufferView.StrideInBytes = m_nStride;
	m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);
}

CTexturedRectMesh::~CTexturedRectMesh()
{
}

CSpriteMesh::CSpriteMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float m_fSize, bool m_bBillBoard)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_nStride = sizeof(CSpriteVertex);
	m_nVertices = 1;
	std::vector<CSpriteVertex> m_BillBordVertices;
	m_BillBordVertices.reserve(1);
	m_BillBordVertices.push_back(CSpriteVertex(XMFLOAT2(m_fSize, m_fSize), m_bBillBoard));

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_BillBordVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dPositionBufferView.StrideInBytes = m_nStride;
	m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);
}

CSpriteMesh::~CSpriteMesh()
{
}

//CSpriteAnimateMesh::CSpriteAnimateMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
//{
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
//	int nSprite = 1;
//	m_nStride = sizeof(CSpriteAnimateVertex);
//	m_nVertices = nSprite;
//	XMFLOAT3 xmf3Position;
//	std::vector< CSpriteAnimateVertex> m_SpriteAnimateVertices;
//	m_SpriteAnimateVertices.reserve(nSprite);
//
//	for (int i = 0; i < nSprite; i++)
//	{
//		float fxTerrain = 0.f;
//		float fzTerrain = 0.f;
//		xmf3Position.x = fxTerrain;
//		xmf3Position.z = fzTerrain + 15.f * i;
//		xmf3Position.y = 0.f;
//
//		m_SpriteAnimateVertices.push_back(CSpriteAnimateVertex(xmf3Position, XMFLOAT2(5.f, 5.f)));
//	}
//
//	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_BillBordVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
//
//	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
//	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
//	m_pd3dPositionBufferView.StrideInBytes = m_nStride;
//	m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
//	m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);
//}
//
//CSpriteAnimateMesh::~CSpriteAnimateMesh()
//{
//}

CParticleMesh::CParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles)
{
	CreateVertexBuffer(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, fLifetime, xmf3Acceleration, xmf3Color, xmf2Size);
	CreateStreamOutputBuffer(pd3dDevice, pd3dCommandList, nMaxParticles);
}

CParticleMesh::~CParticleMesh()
{
	if (m_pd3dStreamOutputBuffer) m_pd3dStreamOutputBuffer->Release();
	if (m_pd3dDrawBuffer) m_pd3dDrawBuffer->Release();
	if (m_pd3dDefaultBufferFilledSize) m_pd3dDefaultBufferFilledSize->Release();
	if (m_pd3dUploadBufferFilledSize) m_pd3dUploadBufferFilledSize->Release();

#ifdef _WITH_QUERY_DATA_SO_STATISTICS
	if (m_pd3dSOQueryBuffer) m_pd3dSOQueryBuffer->Release();
	if (m_pd3dSOQueryHeap) m_pd3dSOQueryHeap->Release();
#else
	if (m_pd3dReadBackBufferFilledSize) m_pd3dReadBackBufferFilledSize->Release();
#endif
}

void CParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size)
{
	m_nVertices = 1;
	m_nStride = sizeof(CParticleVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; // 

	CParticleVertex pVertices[1];

	for (int i = 0; i < 1; ++i)
	{
		pVertices[i].m_xmf3Position = xmf3Position;
		pVertices[i].m_xmf3Velocity = xmf3Velocity;
		pVertices[i].m_fLifetime = fLifetime;
		pVertices[i].m_iType = 0;
	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dPositionBufferView.StrideInBytes = m_nStride;
	m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_pd3dVertexBufferViews.resize(1);
	m_pd3dVertexBufferViews[0] = m_pd3dPositionBufferView;
}

void CParticleMesh::CreateStreamOutputBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nMaxParticles)
{
	m_nMaxParticles = nMaxParticles;

	m_pd3dStreamOutputBuffer = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, NULL, (m_nStride * m_nMaxParticles), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_STREAM_OUT, NULL);
	m_pd3dDrawBuffer = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, NULL, (m_nStride * m_nMaxParticles), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	UINT64 nBufferFilledSize = 0;
	m_pd3dDefaultBufferFilledSize = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, &nBufferFilledSize, sizeof(UINT64), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_STREAM_OUT, NULL);

	m_pd3dUploadBufferFilledSize = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(UINT64), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dUploadBufferFilledSize->Map(0, NULL, (void**)&m_pnUploadBufferFilledSize);

#ifdef _WITH_QUERY_DATA_SO_STATISTICS
	D3D12_QUERY_HEAP_DESC d3dQueryHeapDesc = { };
	d3dQueryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_SO_STATISTICS;
	d3dQueryHeapDesc.Count = 1;
	d3dQueryHeapDesc.NodeMask = 0;
	pd3dDevice->CreateQueryHeap(&d3dQueryHeapDesc, __uuidof(ID3D12QueryHeap), (void**)&m_pd3dSOQueryHeap);

	m_pd3dSOQueryBuffer = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(D3D12_QUERY_DATA_SO_STATISTICS), D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, NULL);
#else
	m_pd3dReadBackBufferFilledSize = ::CreateParticleBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(UINT64), D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, NULL);
#endif
}

void CParticleMesh::PreRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;

	if (nPipelineState == 0)
	{
		if (m_bEmit)
		{
			m_nMaxParticles = m_nMaxParticle;
			m_bEmit = false;
		}
		if (!m_bInitialized)
		{
			m_bInitialized = true;
			m_nVertices = 1;

			m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
			m_pd3dPositionBufferView.StrideInBytes = m_nStride;
			m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
		}
		else
		{
			m_pd3dPositionBufferView.BufferLocation = m_pd3dDrawBuffer->GetGPUVirtualAddress();
			m_pd3dPositionBufferView.StrideInBytes = m_nStride;
			m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
		}
		m_pd3dVertexBufferViews[0] = m_pd3dPositionBufferView;

		m_d3dStreamOutputBufferView.BufferLocation = m_pd3dStreamOutputBuffer->GetGPUVirtualAddress();
		m_d3dStreamOutputBufferView.SizeInBytes = m_nStride * m_nMaxParticles;
		m_d3dStreamOutputBufferView.BufferFilledSizeLocation = m_pd3dDefaultBufferFilledSize->GetGPUVirtualAddress();

		// *m_pnUploadBufferFilledSize = m_nStride * m_nVertices;
		*m_pnUploadBufferFilledSize = 0;

		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dDefaultBufferFilledSize, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_DEST);
		pd3dCommandList->CopyResource(m_pd3dDefaultBufferFilledSize, m_pd3dUploadBufferFilledSize);
		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dDefaultBufferFilledSize, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_STREAM_OUT);
	}
	else if (nPipelineState == 1)
	{
		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dStreamOutputBuffer, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dDrawBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_STREAM_OUT);

		::SwapResourcePointer(&m_pd3dDrawBuffer, &m_pd3dStreamOutputBuffer);

		m_pd3dPositionBufferView.BufferLocation = m_pd3dDrawBuffer->GetGPUVirtualAddress();
		m_pd3dPositionBufferView.StrideInBytes = m_nStride;
		m_pd3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
		m_pd3dVertexBufferViews[0] = m_pd3dPositionBufferView;

	}
}

void CParticleMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nPipelineState)
{
	if (nPipelineState == 0)
	{
		D3D12_STREAM_OUTPUT_BUFFER_VIEW pStreamOutputBufferViews[1] = { m_d3dStreamOutputBufferView };
		pd3dCommandList->SOSetTargets(0, 1, pStreamOutputBufferViews); // // 스트림 출력 버퍼 설정

#ifdef _WITH_QUERY_DATA_SO_STATISTICS
		pd3dCommandList->BeginQuery(m_pd3dSOQueryHeap, D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0, 0);
		CMesh::Render(pd3dCommandList);
		pd3dCommandList->EndQuery(m_pd3dSOQueryHeap, D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0, 0);

		pd3dCommandList->ResolveQueryData(m_pd3dSOQueryHeap, D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0, 0, 1, m_pd3dSOQueryBuffer, 0);
#else
		CMesh::OnPreRender(pd3dCommandList);
		CMesh::Render(pd3dCommandList); //Stream Output to m_pd3dStreamOutputBuffer

		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dDefaultBufferFilledSize, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_SOURCE);
		pd3dCommandList->CopyResource(m_pd3dReadBackBufferFilledSize, m_pd3dDefaultBufferFilledSize);
		::SynchronizeResourceTransition(pd3dCommandList, m_pd3dDefaultBufferFilledSize, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_STREAM_OUT);
#endif
	}
	else if (nPipelineState == 1)
	{
		pd3dCommandList->SOSetTargets(0, 0, NULL); // 스트림 출력 버퍼 설정

		CMesh::OnPreRender(pd3dCommandList);
		CMesh::Render(pd3dCommandList); //Render m_pd3dDrawBuffer 
	}
}

void CParticleMesh::PostRender(ID3D12GraphicsCommandList* pd3dCommandList, UINT nPipelineState)
{
}

//#define _WITH_DEBUG_STREAM_OUTPUT_VERTICES

int CParticleMesh::OnPostRender(int nPipelineState)
{
	if (nPipelineState == 0)
	{
#ifdef _WITH_QUERY_DATA_SO_STATISTICS
		D3D12_RANGE d3dReadRange = { 0, 0 };
		UINT8* pBufferDataBegin = NULL;
		m_pd3dSOQueryBuffer->Map(0, &d3dReadRange, (void**)&m_pd3dSOQueryDataStatistics);
		if (m_pd3dSOQueryDataStatistics) m_nVertices = (UINT)m_pd3dSOQueryDataStatistics->NumPrimitivesWritten;
		m_pd3dSOQueryBuffer->Unmap(0, NULL);
#else
		UINT64* pnReadBackBufferFilledSize = NULL;
		m_pd3dReadBackBufferFilledSize->Map(0, NULL, (void**)&pnReadBackBufferFilledSize);
		m_nVertices = UINT(*pnReadBackBufferFilledSize) / m_nStride;
		m_pd3dReadBackBufferFilledSize->Unmap(0, NULL);
#endif
		::gnCurrentParticles = m_nVertices;
#ifdef _WITH_DEBUG_STREAM_OUTPUT_VERTICES
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Stream Output Vertices = %d\n"), m_nVertices);
		OutputDebugString(pstrDebug);
#endif
		//if ((m_nVertices == 0) || (m_nVertices >= MAX_PARTICLES)) 
		//	m_bEmit = true;
	}

	return m_nVertices;
}

CSkyBoxMesh::CSkyBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nStride = sizeof(XMFLOAT3);
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	// Front
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, +fx));

	// Back
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, -fx));

	// Left							
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, +fx));

	// Right						
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, -fx));

	// Top 									
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fx, +fx));

	// Bottom				
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fx, -fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, +fx));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fx, -fx));


	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_nVertexBufferViews = 1;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

CSkyBoxMesh::~CSkyBoxMesh()
{
}

CTerrainMesh::CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float m_fSize) : CMesh()
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float fx = (m_fSize * 0.5f), fy = 0.f, fz = (m_fSize * 0.5f);

	pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_nVertexBufferViews = 1;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}


CTerrainMesh::~CTerrainMesh()
{
}
