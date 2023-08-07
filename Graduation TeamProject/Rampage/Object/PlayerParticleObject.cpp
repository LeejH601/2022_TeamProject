#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"
#include "PlayerParticleObject.h"

void CPlayerParticleObject::SetSmokeObjects(CGameObject* pSmokeObject)
{
	m_pSmokeObject = pSmokeObject;
	m_pSmokeObject->SetTextureIndex(0, 0);
}

void CPlayerParticleObject::SetTrailParticleObjects(CGameObject* pTrailObjects)
{
	m_pTrailParticleObject = pTrailObjects;
}

void CPlayerParticleObject::SetVertexPointParticleObjects(CGameObject* pVPObject)
{
	m_pVertexPointParicleObject = pVPObject;
}

CGameObject* CPlayerParticleObject::GetSmokeObjects()
{
	return m_pSmokeObject;
}

CGameObject* CPlayerParticleObject::GetTrailParticleObjects()
{
	return m_pTrailParticleObject;
}

CGameObject* CPlayerParticleObject::GetVertexPointParticleObject()
{
	return m_pVertexPointParicleObject;
}
