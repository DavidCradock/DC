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
	class CMessage
	{
	public:
		friend class CMessageUser;

		// Constructor, preparing the message
		CMessage(const String& strMessageContents, EMessageType eMessageType = EMessageType::UNKNOWN);

		// Returns a string holding the message
		const String& getMessageContents(void) const;

		// Returns a string holding the sender's name
		const String& getMessageSender(void) const;

		// Returns the type of this message
		EMessageType getMessageType(void) const;
	private:
		String _mstrContents;		// String holding the message, given to constructor.
		String _mstrSenderName;	// String holding the message's sender's name, given to constructor.
		EMessageType _meType;			// The type of message this is
	};

}