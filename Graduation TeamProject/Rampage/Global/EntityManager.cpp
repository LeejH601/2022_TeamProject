#include "EntityManager.h"

void CEntityManager::RegisterEntity(IEntity* entity)
{
    m_sEntitySet.insert(entity);
}

IEntity* CEntityManager::GetEntity(IEntity* entity)
{

    std::set<IEntity*, EntityComp>::iterator it = m_sEntitySet.find(entity);

    if (it == m_sEntitySet.end())
        return nullptr;

    return *it;
}

void CEntityManager::DeleteEntity(IEntity* entity)
{
    m_sEntitySet.erase(entity);
}
