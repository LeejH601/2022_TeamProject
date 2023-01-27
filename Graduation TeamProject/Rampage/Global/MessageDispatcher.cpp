#include "MessageDispatcher.h"

void CMessageDispatcher::Discharge(IEntity* pReceiver, const Telegram& msg)
{
	if (!pReceiver->HandleMessage(msg)) {
		std::cout << "Message Not Handled" << std::endl;
	}
}

void CMessageDispatcher::DispatchMessages()
{
	while (!PriorityQ.empty())
	{
		Telegram& telegram = const_cast<Telegram&>(*PriorityQ.begin());
		IEntity* pReceiver = nullptr;
		//pReceiver = CObjectManager::GetInst()->GetObjectFromID(telegram.Receiver);
		if (pReceiver)
			Discharge(pReceiver, telegram);
		if (telegram.Extrainfo)
			delete[] telegram.Extrainfo;
		PriorityQ.erase(PriorityQ.begin());
	}
}
