#include "MessageDispatcher.h"
#include "EntityManager.h"
#include "Locator.h"

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
		//pReceiver = Locator.GetEntityManager()->GetEntity(telegram.Receiver);
		if (telegram.Receiver)
			Discharge(telegram.Receiver, telegram);
		if (telegram.Extrainfo)
			delete[] telegram.Extrainfo;
		PriorityQ.erase(PriorityQ.begin());
	}
}
