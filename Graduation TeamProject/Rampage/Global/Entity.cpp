#include "Entity.h"

IEntity::IEntity()
{
	static unsigned int id = 0;
	ID = id++;
}

IEntity::~IEntity()
{
}
