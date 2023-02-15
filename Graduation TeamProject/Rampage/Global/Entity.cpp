#include "Entity.h"
#include "Locator.h"
#include "EntityManager.h"

IEntity::IEntity()
{
	static unsigned int id = 0;
	ID = id++;

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%d \n"), ID);
	OutputDebugString(pstrDebug);

	Locator.GetEntityManager()->RegisterEntity(this);
}

IEntity::~IEntity()
{
	CEntityManager* manager = Locator.GetEntityManager();

	if(manager)
		manager->DeleteEntity(this);

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("delete : %d \n"), ID);
	OutputDebugString(pstrDebug);
}
