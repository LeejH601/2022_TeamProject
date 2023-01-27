#pragma once
#include "stdafx.h"
#include "Entity.h"

class CGameObject;
class CComponent;

struct Telegram
{
	IEntity* Sender;
	IEntity* Receiver;
	int Msg;
	__int64 DispatchTime;
	void* Extrainfo = nullptr;
};

inline bool operator==(const Telegram& t1, const Telegram& t2)
{
	return (fabs(t1.DispatchTime - t2.DispatchTime) < 0) &&
		(t1.Sender->GetID() == t2.Sender->GetID()) &&
		(t1.Receiver->GetID() == t2.Receiver->GetID()) &&
		(t1.Msg == t2.Msg);
}

inline bool operator<(const Telegram& t1, const Telegram& t2)
{
	if (t1 == t2)
	{
		return false;
	}

	else
	{
		return  (t1.DispatchTime < t2.DispatchTime);
	}
}

class CMessageDispatcher
{
	std::set<Telegram> PriorityQ;

	void Discharge(IEntity* pReceiver, const Telegram& msg);

public:
	void DispatchMessages();
};

