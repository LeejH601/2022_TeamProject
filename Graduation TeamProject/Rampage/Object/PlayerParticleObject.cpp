#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"
#include "PlayerParticleObject.h"

void CPlayerParticleObject::SetSmokeObjects(CGameObject* pSmokeObject)
{
	m_pSmokeObject = pSmokeObject;
}

void CPlayerParticleObject::SetTrailObjects(CGameObject* pTrailObjects)
{
	m_pTrailObject = pTrailObjects;
}

CGameObject* CPlayerParticleObject::GetSmokeObjects()
{
	return m_pSmokeObject;
}

CGameObject* CPlayerParticleObject::GetTrailObjects()
{
	return m_pTrailObject;
}
