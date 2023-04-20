#pragma once
#include "../Object/Mesh.h"
#include "../Object/Object.h"
#include "../Object/BillBoardObject.h"
#include "../Global/Camera.h"

#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2


class CPlayerParticleObject : public CGameObject
{
public:
	DECLARE_SINGLE(CPlayerParticleObject);

	void SetSmokeObjects(CGameObject* pSmokeObject);
	void SetTrailParticleObjects(CGameObject* pTrailObjects);

	CGameObject* GetSmokeObjects();
	CGameObject* GetTrailParticleObjects();

private:
	CGameObject* m_pSmokeObject = NULL;
	CGameObject* m_pTrailParticleObject = NULL;
};
