#include "messagingService.h"
#include "messaging.h"

namespace DC
{
	MessageService::MessageService()
	{

	}

	MessageService::~MessageService()
	{

	}

	bool MessageService::isSubscribedToService(const String& userName)
	{
		auto it = subscribedUserNames.begin();
		while (it != subscribedUserNames.end())
		{
			if (*it == userName)
				return true;
			it++;
		}
		return false;
	}

	void MessageService::_receiveMessageFromUser(const Message& message)
	{
		messageStorage.push_back(message);
	}

	void MessageService::dispatch(void)
	{
		SCMessageSystem* pMessageSystem = SCMessageSystem::getPointer();
		// For each message in message storage
		for (size_t iMessage = 0; iMessage < messageStorage.size(); iMessage++)
		{
			// For each subscribed user name
			auto it = subscribedUserNames.begin();
			while (it != subscribedUserNames.end())
			{
				// Get user and send message to it's inbox
				MessageUser* pUser = pMessageSystem->userGet(*it);
				pUser->receiveMessageFromService(messageStorage[iMessage]);
				it++;
			}
		}

		// Empty message storage of service as all messages are now sent
		messageStorage.clear();
	}
}