#include "VertexPointParticleObject.h"
#include "../Global/Locator.h"
#include "../Global/Timer.h"
#include "../Scene/SimulatorScene.h"

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
	this->SetStartAlpha(1.0);
	this->SetColor(XMFLOAT3(0.3803, 0.9372, 0.1098));
	//m_f3Color = Vector3::ScalarProduct(m_f3Color, 5.0f, false);
	this->SetSpeed(15.0f);
	this->SetLifeTime(0.5f);
	this->SetMaxParticleN(MAX_PARTICLES);
	this->SetEmitParticleN(100);
	this->SetParticleType(5);
	this->SetEmissive(20.0f);


	m_fFieldSpeed = 5.0f;
	m_fNoiseStrength = 1.2;
	m_xmf3FieldMainDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fProgressionRate = 1.0f;
	m_fLengthScale = 1.2;
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
	case 0:
	case 5:
		param.m_fLifeTime = m_fLifeTime;
		param.m_fEmitedSpeed = m_fSpeed;
		param.m_iTextureIndex = m_iTextureIndex[0];
		for(int i = 0; i < 3; i++)
		param.m_iTextureCoord[i * 2] = m_iTotalRow[i], param.m_iTextureCoord[i * 2 - 1] = m_iTotalCol[i];

		/*	param.m_fLifeTime = m_fLifeTime;
			param.m_fEmitedSpeed = m_fSpeed;
			param.m_iTextureIndex = m_iTextureIndex;
			param.m_iTextureCoord[0] = m_iTotalRow; param.m_iTextureCoord[1] = m_iTotalCol;*/

		param.m_nEmitNum = m_vVertexPoints.size();
		param.m_xmf3EmiedPositions.resize(m_vVertexPoints.size());
		param.m_xmf3Velocitys.resize(m_vVertexPoints.size());

		param.m_xmf2Size = m_fSize;
		param.m_fEmissive = m_fEmissive;
		param.m_bSimulateRotate = m_bSimulateRotate;
		param.m_bScaleFlag = m_bScaleFlag;
		param.m_xmf4Color = XMFLOAT4(m_f3Color.x, m_f3Color.y, m_f3Color.z, 1.0f);

		if (m_pAnimationContoroller)
		{
			for (int i = 0; i < m_vVertexPoints.size(); ++i) {
				XMFLOAT4X4 mtxVertexToBoneWorld;
				ZeroMemory(&mtxVertexToBoneWorld, sizeof(XMFLOAT4X4));
				int index = m_ivertexPointsIndices[i];

				int BoneIndexs[4] = { m_pMeshBoneIndices[index].x,m_pMeshBoneIndices[index].y,m_pMeshBoneIndices[index].z,m_pMeshBoneIndices[index].w };
				float BoneWeights[4] = { m_pMeshBoneWeights[index].x,m_pMeshBoneWeights[index].y,m_pMeshBoneWeights[index].z,m_pMeshBoneWeights[index].w };
				for (int j = 0; j < 4; j++)
				{
					XMFLOAT4X4 BoneOffset = Matrix4x4::Transpose(m_pMeshBoneOffsets[BoneIndexs[j]]);
					XMFLOAT4X4 BoneTransfrom = Matrix4x4::Transpose(m_pAnimationContoroller->m_ppcbxmf4x4MappedSkinningBoneTransforms[0][BoneIndexs[j]]);
					mtxVertexToBoneWorld = Matrix4x4::Add(mtxVertexToBoneWorld, Matrix4x4::Scale(Matrix4x4::Multiply(BoneOffset, BoneTransfrom), BoneWeights[j]));
				}

				XMFLOAT4 pos = { m_vVertexPoints[i].x,m_vVertexPoints[i].y, m_vVertexPoints[i].z, 1.0f };
				XMStoreFloat3(&param.m_xmf3EmiedPositions[i], XMVector4Transform(XMLoadFloat4(&pos),XMLoadFloat4x4(& mtxVertexToBoneWorld)));
				XMFLOAT4 velocity = {0,1,0 ,0.0f };
				//XMFLOAT4 velocity = { m_vNormals[i].x,m_vNormals[i].y, m_vNormals[i].z ,0.0f };
				XMStoreFloat3(&param.m_xmf3Velocitys[i], XMVector4Transform(XMLoadFloat4(&velocity), XMLoadFloat4x4(&mtxVertexToBoneWorld)));
			}
		}
		else {
			for (int i = 0; i < m_vVertexPoints.size(); ++i) {
				param.m_xmf3EmiedPositions[i] = Vector3::TransformCoord(m_vVertexPoints[i], *m_xmf4x4WorldMatrixReference);
				param.m_xmf3Velocitys[i] = Vector3::Normalize(Vector3::TransformCoord(m_vNormals[i], *m_xmf4x4WorldMatrixReference));
			}
		}
		break;
	default:
		break;
	}
	dynamic_cast<CParticleMesh*>(m_pMesh.get())->EmitParticleForVertexData(emitType, param);
}

void CVertexPointParticleObject::SetVertexPointsFromSkinnedMeshToRandom(CSkinnedMesh* pSkinnedMesh, CAnimationController* pController)
{
	std::vector<XMFLOAT3>& meshVertexs = pSkinnedMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pSkinnedMesh->GetNormals();
	m_pMeshBoneOffsets = pSkinnedMesh->m_pcbxmf4x4MappedBindPoseBoneOffsets;
	m_pMeshBoneIndices = pSkinnedMesh->GetBoneIndices()->data();
	m_pMeshBoneWeights = pSkinnedMesh->GetBoneWeights()->data();
	m_pAnimationContoroller = pController;

	static std::random_device rd;
	static std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, meshVertexs.size() - 1);

	int nVertexPoint = int(meshVertexs.size() / 100);
	nVertexPoint = nVertexPoint < 1 ? 1 : nVertexPoint; // 반드시 1개는 존재하도록 조정

	m_ivertexPointsIndices.resize(nVertexPoint);
	for (int& vertexPoint : m_ivertexPointsIndices) {
		vertexPoint = uid(dre);
	}

	m_vVertexPoints.resize(nVertexPoint);
	m_vNormals.resize(nVertexPoint);
	for (int i = 0; i < nVertexPoint; ++i) {
		m_vVertexPoints[i] = meshVertexs[m_ivertexPointsIndices[i]];
		m_vNormals[i] = meshNormal[m_ivertexPointsIndices[i]];
	}
}

void CVertexPointParticleObject::SetVertexPointsFromSkinnedSubeMeshToRandom(CSkinnedMesh* pSkinnedMesh, UINT subMeshIndex, CAnimationController* pController)
{
	std::vector<XMFLOAT3>& meshVertexs = pSkinnedMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pSkinnedMesh->GetNormals();
	m_pMeshBoneOffsets = pSkinnedMesh->m_pcbxmf4x4MappedBindPoseBoneOffsets;
	m_pMeshBoneIndices = pSkinnedMesh->GetBoneIndices()->data();
	m_pMeshBoneWeights = pSkinnedMesh->GetBoneWeights()->data();
	m_pAnimationContoroller = pController;

	static std::random_device rd;
	static std::default_random_engine dre(rd());

	std::vector<UINT>& subSetIndices = pSkinnedMesh->GetSubSetIndices(subMeshIndex);

	std::uniform_int_distribution<int> uid(0, subSetIndices.size() - 1);

	int nVertexPoint = int(meshVertexs.size() / 10);
	nVertexPoint = nVertexPoint < 1 ? 1 : nVertexPoint; // 반드시 1개는 존재하도록 조정

	m_ivertexPointsIndices.resize(nVertexPoint);
	for (int& vertexPoint : m_ivertexPointsIndices) {
		vertexPoint = subSetIndices[uid(dre)];
	}

	m_vVertexPoints.resize(nVertexPoint);
	m_vNormals.resize(nVertexPoint);
	for (int i = 0; i < nVertexPoint; ++i) {
		m_vVertexPoints[i] = meshVertexs[m_ivertexPointsIndices[i]];
		m_vNormals[i] = meshNormal[m_ivertexPointsIndices[i]];
	}
}



void CVertexPointParticleObject::SetVertexPointsFromStaticMeshToRandom(CMesh* pMesh)
{
	std::vector<XMFLOAT3>& meshVertexs = pMesh->GetVertexs();
	std::vector<XMFLOAT3>& meshNormal = pMesh->GetNormals();

	static std::random_device rd;
	static std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, meshVertexs.size() - 1);

	int nVertexPoint = int(meshVertexs.size());
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
