#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"
#include "PlayerParticleObject.h"

void CPlayerParticleObject::SetSmokeObjects(CGameObject* pSmokeObject)
{
	m_pSmokeObject = pSmokeObject;
}

void CPlayerParticleObject::SetTrailParticleObjects(CGameObject* pTrailObjects)
{
	m_pTrailParticleObject = pTrailObjects;
}

CGameObject* CPlayerParticleObject::GetSmokeObjects()
{
	return m_pSmokeObject;
}

CGameObject* CPlayerParticleObject::GetTrailParticleObjects()
{
	return m_pTrailParticleObject;
}