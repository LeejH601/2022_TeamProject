#pragma once
#include "stdafx.h"
#include "Entity.h"

class EntityComp {
public:
	bool operator()(const IEntity* lhs, const IEntity* rhs) const {
		return lhs->GetID() < rhs->GetID();
	}
};

class CEntityManager
{
	std::set<IEntity*> m_sEntitySet;

public:
	void RegisterEntity(IEntity* entity);
	IEntity* GetEntity(IEntity* entity);
	void DeleteEntity(IEntity* entity);
	void Clear() { m_sEntitySet.clear(); };
};

