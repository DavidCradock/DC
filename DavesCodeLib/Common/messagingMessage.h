#pragma once
#include "string.h"

namespace DC
{
	// Message types
	// Used when constructing a message to set it's type.
	// Used to differenciate between different types of messages.
	// Add more here if you need them
	enum EMessageType
	{
		UNKNOWN,	// Unknown type
		WINDOW		// Window related message
	};

	// A message object used by a messaging system and message user
	class Message
	{
	public:
		friend class MessageUser;

		// Constructor, preparing the message
		Message(const String& messageContents, EMessageType messageType = EMessageType::UNKNOWN);

		// Returns a string holding the message
		const String& getMessageContents(void) const;

		// Returns a string holding the sender's name
		const String& getMessageSender(void) const;

		// Returns the type of this message
		EMessageType getMessageType(void) const;
	private:
		String contents;		// String holding the message, given to constructor.
		String senderName;		// String holding the message's sender's name.
		EMessageType type;		// The type of message this is, given to constructor.
	};

}