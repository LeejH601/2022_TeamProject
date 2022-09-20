#pragma once

//----------------------------------------------------------------------------
// 22/09/20 CMesh 클래스 추가 - Leejh
//----------------------------------------------------------------------------

class CMesh
{
public:
	CMesh() {};
	virtual ~CMesh() {};

protected:
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nType = 0;

public:
	UINT GetType() { return m_nType; };
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet) { }
};

