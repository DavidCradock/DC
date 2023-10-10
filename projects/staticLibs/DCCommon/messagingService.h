#pragma once
#include "messagingMessage.h"
#include "messagingUser.h"
#include <list>

namespace DC
{
	// A message system service
	// A message service is responsible for receiving and sending messages from/to CMessageUsers.
	class MessageService
	{
		friend class SCMessageSystem;
		friend class MessageUser;
	public:
		MessageService();

		~MessageService();

		// Returns whether the given user is subscribed to this service or not
		bool isSubscribedToService(const String& userName);

		// Dispatches all messages to all subscribed users
		// Once this is called, all messages are removed from this service's message storage.
		void dispatch(void);

	private:
		// List of users who are subscribed to this service
		std::list<String> subscribedUserNames;

		// Temporarily holds all received messages from users
		std::vector<Message> messageStorage;

		// Receives a new message from a user which may or may not be subscribed to this service to be sent via dispatch() to all subscribed users.
		void _receiveMessageFromUser(const Message& message);
	};
}