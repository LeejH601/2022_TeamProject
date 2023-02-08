#pragma once
#include "stdafx.h"

class Telegram;

class CComponent
{
protected:
	bool m_bEnable = false;

public:
	CComponent() = default;
	virtual ~CComponent() = default;

	virtual void Update(float fElapsedTime) { };
	virtual bool Init() { return true; };
	virtual void Reset() {};
	virtual bool HandleMessage(const Telegram& msg) { return true; };

	void SetEnable(bool flag) { m_bEnable = flag; };
	bool& GetEnable() { return m_bEnable; };
};

typedef std::pair<size_t, std::shared_ptr<CComponent>> ComponentPair;

//bool operator==(const ComponentPair& lhs, const ComponentPair& rhs) 
//{
//	return strcmp(lhs.first.name(), rhs.first.name()) == 0;
//}

class Comp_Compair {
public:
	bool operator()(const ComponentPair& lhs, const ComponentPair& rhs) const
	{
		return lhs.first < rhs.first;
	}
};

class CComponentSet
{
	std::set<ComponentPair, Comp_Compair> m_sComponents;

public:
	CComponentSet() = default;
	virtual ~CComponentSet() = default;

	CComponent* FindComponent(const type_info& type);
	CComponent* FindComponent(const size_t& hashcode);
	void AddComponent(std::shared_ptr<CComponent>& component);

	std::set<ComponentPair, Comp_Compair>& GetComponentSet() { return m_sComponents; };
};