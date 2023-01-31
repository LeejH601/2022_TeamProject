#include "Locator.h"
#include "Global.h"
#include "Component.h"
#include "Camera.h"

bool CLocator::Init()
{
	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);

	for (auto& [num, componentSet] : m_sComponentSets) {
		std::shared_ptr<CComponent> component = std::make_shared<CCameraMover>(m_pSimulaterCamera);
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraShaker>(m_pSimulaterCamera);
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraZoomer>(m_pSimulaterCamera);
		componentSet->AddComponent(component);
	}

	return true;
}
