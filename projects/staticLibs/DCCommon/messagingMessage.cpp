#include "messagingMessage.h"

namespace DC
{
	CMessage::CMessage(const String& strMessageContents, EMessageType eMessageType)
	{
		_mstrContents = strMessageContents;
		_mstrSenderName = L"UNKNOWN";
		_meType = eMessageType;
	}

	const String& CMessage::getMessageContents(void) const
	{
		return _mstrContents;
	}

	const String& CMessage::getMessageSender(void) const
	{
		return _mstrSenderName;
	}

	EMessageType CMessage::getMessageType(void) const
	{
		return _meType;
	}
}