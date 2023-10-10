#pragma once
#include "singleton.h"
#include "messagingService.h"
#include "templateManager.h"

namespace DC
{
	// The message system is responsible for handling everything message related.
	// There are three other classes in the messaging system and they are...
	// Message which represents a single message that is passed around the messaging system.
	// MessageUser which represents a single message user which can send and receive messages to and from a...
	// MessageService which is responsible for receiving and sending messages to/from CMessageUsers.
	// Important note: Once a user is cacpable of receiving messages, by being subscribed to service, that user
	// needs to check it's inbox and empty it on a regular basis, otherwise the inbox will eventually fill up with loads of messages.
	// To empty the inbox, either call the user's getMessageFromInbox() until it's doesInboxContainMessages() returns false, or
	// call the user's getMessagesFromInbox() method. Alternatively, a call to that user's emptyInbox() will suffice.
	/* Example usage :
		// Setup messaging system
		// Create a message service which handles sending and receiving of messages to/from message users
		MessageService* pMsgService = x->pMessageSystem->serviceAdd("service");	// Create a messaging service
		MessageUser* pUserDavid = x->pMessageSystem->userAdd("David");				// Add a user
		MessageUser* pUserMojo = x->pMessageSystem->userAdd("Mojo");				// Add another user
		x->pMessageSystem->subscribeUserToService("David", "service");				// Subscribe "David" user to "service" service.
		Message msg("ThisIsMessageContents", EMessageType::UNKNOWN);				// Create a message for some user to send to the service
		pUserMojo->postNewMessage(msg, "service");									// Let user "Mojo" send the message to the "service" service
		pMsgService->dispatch();													// Get the service to dispatch all received messages to all subscribed users
		while (pUserMojo->doesInboxContainMessages())								// Get Mojo user to check to see if their inbox contains any messages
			Message message = pUserMojo->getMessageFromInbox();					// If there's a message, remove it from the user's inbox and then we can read the message
		// There won't be any messages as user Mojo is not subscribed to any service.
		if (pUserDavid->doesInboxContainMessages())									// Get David user to check to see if their inbox contains any messages
		{
			std::vector<Message> messages = pUserDavid->getMessagesFromInbox();	// Get all messages from the user's inbox, emptying the inbox in the process.
			for (size_t iMessage = 0; iMessage < messages.size(); iMessage++)		// Do stuff with the messages
			{
				Message mes = messages[iMessage];
				if (mes.getMessageSender() == "Mojo")
				{
					String strContents = mes.getMessageContents();
				}
			}
		}
	*/
	class SCMessageSystem : public Singleton<SCMessageSystem>
	{
	public:
		SCMessageSystem();

		// Adds a new message service to the system and returns a pointer to it if needed
		// If the named service already exists, an exception occurs.
		MessageService* serviceAdd(const String& serviceName);

		// Removes a message service from the system
		// If the named service does not exist, an exception occurs.
		void serviceRemove(const String& serviceName);

		// Returns whether the named service currently exists or not
		bool serviceExists(const String& serviceName);

		// Returns a pointer to the named message service.
		// If the named message system does not exist, an exception occurs
		MessageService* serviceGet(const String& serviceName);

		// Adds a new message user to the system and returns a pointer to it if needed
		// If the named user already exists, an exception occurs.
		MessageUser* userAdd(const String& userName);

		// Removes a message user from the system
		// If the named user does not exist, an exception occurs.
		void userRemove(const String& userName);

		// Returns whether the named user currently exists or not
		bool userExists(const String& userName);

		// Returns a pointer to the named message user.
		// If the named message user does not exist, an exception occurs
		MessageUser* userGet(const String& userName);

		// Subscribes the named message user to the named service
		// If the user does not exist, an exception occurs.
		// If the service does not exist, an exception occurs.
		// If the user is already subscribed, nothing happens.
		void subscribeUserToService(const String& userName, const String& serviceName);

		// Unsubscribes the named message user from the named service
		// If the user does not exist, an exception occurs.
		// If the service does not exist, an exception occurs.
		// If the user is already unsubscribed, nothing happens.
		void unsubscribeUserFromService(const String& userName, const String& serviceName);

	private:
		Manager<MessageService> services;
		Manager<MessageUser> users;
	};
}