#pragma once
#include "Object.h"

struct ObjectInfo {
	BoundingOrientedBox* m_OBB;
	CGameObject* m_pObject;
};

class CollisionChecker {
	std::vector<ObjectInfo> m_InfoList;
public:
	void UpdateObjects(float fTimeElapsed);
	void RegisterObject(CGameObject* pObject);
};