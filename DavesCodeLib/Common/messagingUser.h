#pragma once
#include "messagingMessage.h"
#include "messagingService.h"
#include <list>

namespace DC
{
	// A message user
	// Can send and receive messages from a message system.
	// By default, a message user isn't subscribed to any message systems, use subscribe() to become subscribed
	// to a message system and be able to send and receive messages to that system.
	class MessageUser
	{
	public:
		friend class SCMessageSystem;
		friend class MessageService;
		MessageUser();

		// Returns whether this user is subscribed to the named service or not
		bool isSubscribedToService(const String& serviceName);

		// Posts a new message to the given service.
		// The service will then, send that message to all subscribed users of that service upon calling it's dispatch() method.
		// If the named service doesn't exist, an exception occurs.
		// This also fills the newMessage with this user's name.
		void postNewMessage(Message& newMessage, const String& serviceName);

		// Returns true if there are any messages in this user's inbox
		bool doesInboxContainMessages(void);

		// Gets a message from this user's inbox, removing it from the inbox.
		// These messages are sent from any services this user is subscribed to.
		// If there are no messages to get, an exception occurs. Use doesInboxContainMessages() to prevent this.
		Message getMessageFromInbox(void);

		// Gets all messages from this user's inbox, emptying the inbox.
		// These messages are sent from any services this user is subscribed to.
		// If there are no messages to get, the return vector of messages is empty
		// The messages that were received first are at the beginning of the returned vector.
		std::vector<Message> getMessagesFromInbox(void);
		
		// Simply empties this user's inbox, discarding any received messages
		void emptyInbox(void);
	private:
		String name;			// Name of this message user.

		// List of names this user is subscribed to
		std::list<String> subscribedServiceNames;

		// Receives a new message from a service which this user is subscribed to and place it in this user's inbox
		void receiveMessageFromService(const Message& message);

		// Messages are added to the end of this list from any subscribed services when that service's dispatch method is called.
		// Calling getMessages() empties this vector
		std::list<Message> inbox;
	};
}