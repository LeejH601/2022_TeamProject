#include "CollisionChecker.h"

void CollisionChecker::UpdateObjects(float fTimeElapsed)
{
}

void CollisionChecker::RegisterObject(CGameObject* pObject)
{
	ObjectInfo objInfo{};
	objInfo.m_OBB = pObject->GetBoundingBox();
	objInfo.m_pObject = pObject;

	m_InfoList.emplace_back(objInfo);
}
