#include "Entity.h"

IEntity::IEntity()
{
	static unsigned int id = 0;
	ID = id++;

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%d \n"), ID);
	OutputDebugString(pstrDebug);
}

IEntity::~IEntity()
{
}
