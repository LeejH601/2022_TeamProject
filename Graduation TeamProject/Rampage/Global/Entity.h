#pragma once
#include "stdafx.h"

class Telegram;

class IEntity
{
private:
	unsigned int ID;
public:
	IEntity();
	virtual ~IEntity();
public:
	const unsigned int GetID() const { return ID; };

	virtual bool HandleMessage(const Telegram& msg) { return true; };

	bool operator<(const IEntity* rhs) {
		return this->ID < rhs->ID;
	}
	bool operator<(const IEntity& rhs) {
		return this->ID < rhs.ID;
	}

};