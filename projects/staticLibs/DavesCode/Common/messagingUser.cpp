#include "messagingUser.h"
#include "messaging.h"

namespace DC
{
	MessageUser::MessageUser()
	{
		name = L"UNNAMED";
	}

	bool MessageUser::isSubscribedToService(const String& serviceName)
	{
		auto it = subscribedServiceNames.begin();
		while (it != subscribedServiceNames.end())
		{
			if (*it == serviceName)
				return true;
			it++;
		}
		return false;
	}

	void MessageUser::postNewMessage(Message& newMessage, const String& serviceName)
	{
		SCMessageSystem* pMessageSystem = SCMessageSystem::getPointer();
		ErrorIfFalse(pMessageSystem->serviceExists(serviceName), L"MessageUser::postNewMessage() failed. The given service to send the message to called \"" + serviceName + L"\" does not exist.");
		MessageService* pService = pMessageSystem->serviceGet(serviceName);

		// Set name of the sender to this user
		newMessage.senderName = name;

		// Set message to the named service
		pService->_receiveMessageFromUser(newMessage);
	}

	void MessageUser::receiveMessageFromService(const Message& message)
	{
		inbox.push_back(message);
	}

	bool MessageUser::doesInboxContainMessages(void)
	{
		return inbox.size() > 0;
	}

	Message MessageUser::getMessageFromInbox(void)
	{
		ErrorIfTrue(!inbox.size(), L"MessageUser::getMessageFromInbox() failed. There are no more messages in the inbox.");
		Message message = *inbox.begin();
		inbox.pop_front();
		return message;
	}

	std::vector<Message> MessageUser::getMessagesFromInbox(void)
	{
		std::vector<Message> messages;
		auto it = inbox.begin();
		while (it != inbox.end())
		{
			messages.push_back(*it);
			it++;
		}
		inbox.clear();
		return messages;
	}

	void MessageUser::emptyInbox(void)
	{
		inbox.clear();
	}
}