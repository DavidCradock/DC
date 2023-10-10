#include "messagingUser.h"
#include "messaging.h"

namespace DC
{
	CMessageUser::CMessageUser()
	{
		_mstrName = L"UNNAMED";
	}

	bool CMessageUser::isSubscribedToService(const String& strServiceName)
	{
		auto it = _mlstSubscribedServiceNames.begin();
		while (it != _mlstSubscribedServiceNames.end())
		{
			if (*it == strServiceName)
				return true;
			it++;
		}
		return false;
	}

	void CMessageUser::postNewMessage(CMessage& newMessage, const String& strServiceName)
	{
		SCMessageSystem* pMessageSystem = SCMessageSystem::getPointer();
		ErrorIfFalse(pMessageSystem->serviceExists(strServiceName), L"MessageUser::postNewMessage() failed. The given service to send the message to called \"" + strServiceName + L"\" does not exist.");
		CMessageService* pService = pMessageSystem->serviceGet(strServiceName);

		// Set name of the sender to this user
		newMessage._mstrSenderName = _mstrName;

		// Set message to the named service
		pService->_receiveMessageFromUser(newMessage);
	}

	void CMessageUser::_receiveMessageFromService(const CMessage& message)
	{
		_mlistInbox.push_back(message);
	}

	bool CMessageUser::doesInboxContainMessages(void)
	{
		return _mlistInbox.size() > 0;
	}

	CMessage CMessageUser::getMessageFromInbox(void)
	{
		ErrorIfTrue(!_mlistInbox.size(), L"MessageUser::getMessageFromInbox() failed. There are no more messages in the inbox.");
		CMessage message = *_mlistInbox.begin();
		_mlistInbox.pop_front();
		return message;
	}

	std::vector<CMessage> CMessageUser::getMessagesFromInbox(void)
	{
		std::vector<CMessage> messages;
		auto it = _mlistInbox.begin();
		while (it != _mlistInbox.end())
		{
			messages.push_back(*it);
			it++;
		}
		_mlistInbox.clear();
		return messages;
	}

	void CMessageUser::emptyInbox(void)
	{
		_mlistInbox.clear();
	}
}