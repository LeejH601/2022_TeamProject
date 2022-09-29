#include "stdafx.h"
#include "Mesh.h"

//----------------------------------------------------------------------------
// 22/09/21 Mesh �Լ��� ���� - ChoiHS
//----------------------------------------------------------------------------

// Mesh ������
CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	// pstrFileName�� ������ �̸��� ���ڷ� �Ѿ���� pstrFileName���� LoadMeshFromFile�� ȣ��
	if (pstrFileName) LoadMeshFromFile(pd3dDevice, pd3dCommandList, pstrFileName);
}

CMesh::~CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	// UploadBuffer�� Default���� ������ ������ �� �ҷ����� �Լ�
	// ������Ʈ�� ������ �� �ʿ������ UploadBuffer���� Release() �ϴ� �Լ�
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
	// �⺻���� �������� �ﰢ������Ʈ�� ����
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// ���� ���� �並 ������������ �Է� ���Կ� binding �Ѵ�.
	// ù��° ���ڴ� �Է� ����, �ι�°�� ���� ���� ����, ����°�� ���� ���� ���� ù ���Ҹ� ����Ű�� �������̴�.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexBufferViews, m_pd3dVertexBufferViews.data());
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	// �޽��� �������ϴ� �Լ��̴�.
	
	if (m_nSubsets > 0)
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[nSubset]);
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubset], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
#ifdef _WITH_TEXT_MESH
	ifstream InFile(pstrFileName);

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!strcmp(pstrToken, "<Vertices>:"))
		{
			InFile >> m_nVertices;
			m_pxmf3Positions = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Positions[i].x >> m_pxmf3Positions[i].y >> m_pxmf3Positions[i].z;
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			InFile >> pstrToken;
			m_pxmf3Normals = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Normals[i].x >> m_pxmf3Normals[i].y >> m_pxmf3Normals[i].z;
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			InFile >> m_nIndices;
			m_pnIndices = new UINT[m_nIndices];
			for (UINT i = 0; i < m_nIndices; i++) InFile >> m_pnIndices[i];
		}
	}
#else
	FILE* pFile = NULL;
	::fopen_s(&pFile, pstrFileName, "rb");
	::rewind(pFile);

	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	while (!::feof(pFile))
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pFile);
		if (nReads == 0) break;
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<BoundingBox>:"))
		{
			nReads = (UINT)::fread(&m_xmBoundingBox.Center, sizeof(float), 3, pFile);
			nReads = (UINT)::fread(&m_xmBoundingBox.Extents, sizeof(float), 3, pFile);
		}
		else if (!strcmp(pstrToken, "<Vertices>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
			m_pxmf3Positions.resize(m_nVertices);
			nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(float), 3 * m_nVertices, pFile);
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
			m_pxmf3Normals.resize(m_nVertices);
			nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(float), 3 * m_nVertices, pFile);
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nReads = (UINT)::fread(&m_nIndices, sizeof(int), 1, pFile);
			m_pnIndices.resize(m_nIndices);
			nReads = (UINT)::fread(m_pnIndices.data(), sizeof(UINT), m_nIndices, pFile);
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&m_nSubsets, sizeof(int), 1, pFile);

			m_pnSubSetIndices.resize(m_nSubsets);
			m_pnSubSetStartIndices.resize(m_nSubsets);
			m_ppnSubSetIndices.resize(m_nSubsets);

			for (UINT i = 0; i < m_nSubsets; i++)
			{
				nReads = (UINT)::fread(&m_pnSubSetStartIndices[i], sizeof(UINT), 1, pFile);
				nReads = (UINT)::fread(&m_pnSubSetIndices[i], sizeof(UINT), 1, pFile);
				nReads = (UINT)::fread(&m_nIndices, sizeof(int), 1, pFile);
				m_ppnSubSetIndices[i].resize(m_pnSubSetIndices[i]);
				nReads = (UINT)::fread(m_ppnSubSetIndices[i].data(), sizeof(UINT), m_pnSubSetIndices[i], pFile);
			}

			break;
		}
	}

	::fclose(pFile);
#endif

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_nVertexBufferViews = 2;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_ppd3dIndexBuffers.resize(m_nSubsets);
	m_ppd3dIndexUploadBuffers.resize(m_nSubsets);
	m_pd3dIndexBufferViews.resize(m_nSubsets);

	for (UINT i = 0; i < m_nSubsets; i++)
	{
		m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

		m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
		m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
		m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
	}
}

//========================== �ӽ� �޽� ==========================
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//������ü�� ������(����)�� 8���̴�. 
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, +fz));

	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	m_pxmf3Colors.push_back(XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Colors.data(), sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_nSubsets = 1;
	m_nVertexBufferViews = 2;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT4);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;

	m_nIndices = 36;

	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(3); m_pnIndices.push_back(1); m_pnIndices.push_back(0);
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(2); m_pnIndices.push_back(1); m_pnIndices.push_back(3);
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(0); m_pnIndices.push_back(5); m_pnIndices.push_back(4);
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(1); m_pnIndices.push_back(5); m_pnIndices.push_back(0);
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(3); m_pnIndices.push_back(4); m_pnIndices.push_back(7);
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(0); m_pnIndices.push_back(4); m_pnIndices.push_back(3);
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(1); m_pnIndices.push_back(6); m_pnIndices.push_back(5);
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(2); m_pnIndices.push_back(6); m_pnIndices.push_back(1);
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(2); m_pnIndices.push_back(7); m_pnIndices.push_back(6);
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(3); m_pnIndices.push_back(7); m_pnIndices.push_back(2);
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices.push_back(6); m_pnIndices.push_back(4); m_pnIndices.push_back(5);
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices.push_back(7); m_pnIndices.push_back(4); m_pnIndices.push_back(6);

	m_nSubsets = 1;

	m_ppd3dIndexBuffers.resize(m_nSubsets);
	m_ppd3dIndexUploadBuffers.resize(m_nSubsets);
	m_pd3dIndexBufferViews.resize(m_nSubsets);
	m_pnSubSetIndices.resize(m_nSubsets);

	m_pnSubSetIndices[0] = m_nIndices;
	//�ε��� ���۸� �����Ѵ�. 
	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices.data(), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	//�ε��� ���� �並 �����Ѵ�. 
	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}