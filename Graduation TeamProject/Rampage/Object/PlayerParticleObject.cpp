#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"
#include "PlayerParticleObject.h"

void CPlayerParticleObject::SetSmokeObjects(std::vector<std::unique_ptr<CGameObject>>* pSmokeObjects)
{
	m_pSmokeObjects = pSmokeObjects;
}

void CPlayerParticleObject::SetTrailObjects(std::vector<std::unique_ptr<CGameObject>>* pTrailObjects)
{
	m_pTrailObjects = pTrailObjects;
}

std::vector<std::unique_ptr<CGameObject>>* CPlayerParticleObject::GetSmokeObjects()
{
	return m_pSmokeObjects;
}

std::vector<std::unique_ptr<CGameObject>>* CPlayerParticleObject::GetTrailObjects()
{
	return m_pTrailObjects;
}
