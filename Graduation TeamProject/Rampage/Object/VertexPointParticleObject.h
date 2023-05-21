#pragma once
#include "ParticleObject.h"

class CVertexPointParticleObject : public CParticleObject
{
	std::vector<XMFLOAT3> m_vVertexPoints;
	std::vector<XMFLOAT3> m_vNormals;
	XMFLOAT4X4* m_xmf4x4WorldMatrixReference = nullptr;

public:
	CVertexPointParticleObject();
	CVertexPointParticleObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType);
	virtual ~CVertexPointParticleObject();

	virtual void Update(float fTimeElapsed);
	void EmitParticle(int emitType);

	void SetWorldMatrixReference(XMFLOAT4X4* xmf4x4WorldReference) { m_xmf4x4WorldMatrixReference = xmf4x4WorldReference; };
	void SetVertexPointsFromSkinnedMeshToRandom(CSkinnedMesh* pSkinnedMesh);
	void SetVertexPointsFromStaticMeshToRandom(CMesh* pMesh);
	void SetVertexPointsFromStaticMeshToUniform(CMesh* pMesh);
};

