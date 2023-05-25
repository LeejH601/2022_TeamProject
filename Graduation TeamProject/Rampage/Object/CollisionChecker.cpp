#include "CollisionChecker.h"
#include "PhysicsObject.h"

void CollisionChecker::UpdateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_InfoList.size() - 1; ++i)
	{
		if (!m_InfoList[i].m_pObject->m_bEnable)
			continue;

		for (int j = i + 1; j < m_InfoList.size(); ++j)
		{
			if (m_InfoList[j].m_pObject->m_bEnable && 
				m_InfoList[i].m_OBB->Intersects(*(m_InfoList[j].m_OBB)))
			{
				//Object들이 충돌하면 여기서 처리
				static_cast<CPhysicsObject*>(m_InfoList[i].m_pObject)->DistortLookVec(m_InfoList[j].m_pObject);
				static_cast<CPhysicsObject*>(m_InfoList[j].m_pObject)->DistortLookVec(m_InfoList[i].m_pObject);
			}
		}
	}
}

void CollisionChecker::RegisterObject(CGameObject* pObject)
{
	pObject->Update(0.0f);

	ObjectInfo objInfo{};
	objInfo.m_OBB = pObject->GetBoundingBox();
	objInfo.m_pObject = pObject;

	m_InfoList.emplace_back(objInfo);
}
