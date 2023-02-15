#include "Component.h"
#include "Locator.h"

CComponent* CComponentSet::FindComponent(const type_info& type)
{
    return FindComponent(type.hash_code());
}

CComponent* CComponentSet::FindComponent(const size_t& hashcode)
{
    static ComponentPair dummy(0, std::shared_ptr<CComponent>());

    dummy.first = hashcode;
    std::set<ComponentPair, Comp_Compair>::iterator it = m_sComponents.find(dummy);

    if (it == m_sComponents.end())
        return nullptr;

    return it->second.get();
}

void CComponentSet::AddComponent(std::shared_ptr<CComponent>& component)
{
    ComponentPair pair = std::make_pair(typeid(*(component.get())).hash_code(), component);

    m_sComponents.insert(pair);
}