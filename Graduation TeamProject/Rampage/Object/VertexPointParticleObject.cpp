#include "VertexPointParticleObject.h"
#include "../Global/Locator.h"
#include "../Global/Timer.h"

CVertexPointParticleObject::CVertexPointParticleObject() : CParticleObject()
{
}

CVertexPointParticleObject::CVertexPointParticleObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration,
	XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType) :
	CParticleObject(iTextureIndex, pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, xmf3Position, xmf3Velocity, fLifetime, xmf3Acceleration,
		xmf3Color, xmf2Size, nMaxParticles, pShader, iParticleType)
{
	this->SetEmit(true);
	this->SetSize(XMFLOAT2(0.2, 0.2));
	this->SetStartAlpha(0.5);
	this->SetColor(XMFLOAT3(0.3, 0.1, 0.1));
	this->SetSpeed(10.0f);
	this->SetLifeTime(2.0f);
	this->SetMaxParticleN(MAX_PARTICLES);
	this->SetEmitParticleN(100);
	this->SetParticleType(5);
}

CVertexPointParticleObject::~CVertexPointParticleObject()
{
}

void CVertexPointParticleObject::Update(float fTimeElapsed)
{

}

void CVertexPointParticleObject::EmitParticle(int emitType)
{
	ParticleEmitPositionlistParam param;
	switch (emitType)
	{
	case 5:
		param.m_fLifeTime = 0.5f;
		param.m_fEmitedSpeed = 5.0f;
		param.m_iTextureIndex = 1;
		param.m_iTextureCoord[0] = m_iTotalRow; param.m_iTextureCoord[1] = m_iTotalCol;

		/*	param.m_fLifeTime = m_fLifeTime;
			param.m_fEmitedSpeed = m_fSpeed;
			param.m_iTextureIndex = m_iTextureIndex;
			param.m_iTextureCoord[0] = m_iTotalRow; param.m_iTextureCoord[1] = m_iTotalCol;*/

		param.m_nEmitNum = m_vVertexPoints.size();
		param.m_xmf3EmiedPositions.resize(m_vVertexPoints.size());
		param.m_xmf3Velocitys.resize(m_vVertexPoints.size());
		for (int i = 0; i < m_vVertexPoints.size(); ++i) {
			param.m_xmf3EmiedPositions[i] = Vector3::TransformCoord(m_vVertexPoints[i], *m_xmf4x4WorldMatrixReference);
			param.m_xmf3Velocitys[i] = Vector3::Normalize(Vector3::TransformCoord(m_vNormals[i], *m_xmf4x4WorldMatrixReference));
		}
		break;
	default:
		break;
	}
	dynamic_cast<CParticleMesh*>(m_pMesh.get())->EmitParticleForVertexData(emitType, param);
}

void CVertexPointParticleObject::SetVertexPointsFromSkinnedMeshToRandom(CSkinnedMesh* pSkinnedMesh)
{
	std::vector<XMFLOAT3>& meshVertexs = pSkinnedMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pSkinnedMesh->GetNormals();

	static std::random_device rd;
	static std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, meshVertexs.size() - 1);

	int nVertexPoint = int(meshVertexs.size() / 10);
	nVertexPoint = nVertexPoint < 1 ? 1 : nVertexPoint; // 반드시 1개는 존재하도록 조정

	std::vector<int> vertexPointsIndices(nVertexPoint);
	for (int& vertexPoint : vertexPointsIndices) {
		vertexPoint = uid(dre);
	}

	m_vVertexPoints.resize(nVertexPoint);
	m_vNormals.resize(nVertexPoint);
	for (int i = 0; i < nVertexPoint; ++i) {
		m_vVertexPoints[i] = meshVertexs[vertexPointsIndices[i]];
		m_vNormals[i] = meshNormal[vertexPointsIndices[i]];
	}
}

void CVertexPointParticleObject::SetVertexPointsFromStaticMeshToRandom(CMesh* pMesh)
{
	std::vector<XMFLOAT3>& meshVertexs = pMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pMesh->GetNormals();

	static std::random_device rd;
	static std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, meshVertexs.size() - 1);

	int nVertexPoint = int(meshVertexs.size() / 10);
	nVertexPoint = nVertexPoint < 1 ? 1 : nVertexPoint; // 반드시 1개는 존재하도록 조정

	std::vector<int> vertexPointsIndices(nVertexPoint);
	for (int& vertexPoint : vertexPointsIndices) {
		vertexPoint = uid(dre);
	}

	m_vVertexPoints.resize(nVertexPoint);
	m_vNormals.resize(nVertexPoint);
	for (int i = 0; i < nVertexPoint; ++i) {
		m_vVertexPoints[i] = meshVertexs[vertexPointsIndices[i]];
		m_vNormals[i] = meshNormal[vertexPointsIndices[i]];
	}
}

void CVertexPointParticleObject::SetVertexPointsFromStaticMeshToUniform(CMesh* pMesh)
{
	std::vector<XMFLOAT3>& meshVertexs = pMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pMesh->GetNormals();


	int nVertexPoint = int(meshVertexs.size() / 10);
	nVertexPoint = nVertexPoint < 1 ? 1 : nVertexPoint; // 반드시 1개는 존재하도록 조정

	std::vector<int> vertexPointsIndices(nVertexPoint);
	int index = 0;
	for (int& vertexPoint : vertexPointsIndices) {
		vertexPoint = index;
		index += 10;
	}

	m_vVertexPoints.resize(nVertexPoint);
	m_vNormals.resize(nVertexPoint);
	for (int i = 0; i < nVertexPoint; ++i) {
		m_vVertexPoints[i] = meshVertexs[vertexPointsIndices[i]];
		m_vNormals[i] = meshNormal[vertexPointsIndices[i]];
	}
}
