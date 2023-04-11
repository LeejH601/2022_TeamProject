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

	void SetSmokeObjects(std::vector<std::unique_ptr<CGameObject>>* pSmokeObjects);
	void SetTrailObjects(std::vector<std::unique_ptr<CGameObject>>* pTrailObjects);

	std::vector<std::unique_ptr<CGameObject>>* GetSmokeObjects();
	std::vector<std::unique_ptr<CGameObject>>* GetTrailObjects();

private:
    std::vector<std::unique_ptr<CGameObject>>* m_pSmokeObjects = NULL;
	std::vector<std::unique_ptr<CGameObject>>* m_pTrailObjects = NULL;
};
