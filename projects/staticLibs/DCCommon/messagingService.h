#pragma once
#include "messagingMessage.h"
#include "messagingUser.h"
#include <list>

namespace DC
{
	// A message system service
	// A message service is responsible for receiving and sending messages from/to CMessageUsers.
	class CMessageService
	{
		friend class SCMessageSystem;
		friend class CMessageUser;
	public:
		CMessageService();

		~CMessageService();

		// Returns whether the given user is subscribed to this service or not
		bool isSubscribedToService(const String& strUserName);

		// Dispatches all messages to all subscribed users
		// Once this is called, all messages are removed from this service's message storage.
		void dispatch(void);

	private:
		// List of users who are subscribed to this service
		std::list<String> _mlstSubscribedUserNames;

		// Temporarily holds all received messages from users
		std::vector<CMessage> _mvecMessageStorage;

		// Receives a new message from a user which may or may not be subscribed to this service to be sent via dispatch() to all subscribed users.
		void _receiveMessageFromUser(const CMessage& message);
	};
}