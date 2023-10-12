#include "messagingMessage.h"

namespace DC
{
	Message::Message(const String& messageContents, EMessageType messageType)
	{
		contents = messageContents;
		senderName = L"UNKNOWN";
		type = messageType;
	}

	const String& Message::getMessageContents(void) const
	{
		return contents;
	}

	const String& Message::getMessageSender(void) const
	{
		return senderName;
	}

	EMessageType Message::getMessageType(void) const
	{
		return type;
	}
}