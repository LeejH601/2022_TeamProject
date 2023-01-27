#pragma once
#include "stdafx.h"

class Telegram;

class IEntity
{
	unsigned int ID;

public:
	IEntity();
	virtual ~IEntity();

	const unsigned int GetID() { return ID; };

	virtual bool HandleMessage(const Telegram& msg) { return true; };
};

