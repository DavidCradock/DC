#include "messaging.h"

namespace DC
{
	SCMessageSystem::SCMessageSystem()
	{
	}

	CMessageService* SCMessageSystem::serviceAdd(const String& strServiceName)
	{
		ErrorIfTrue(services.exists(strServiceName), L"MessageSystem::serviceAdd(\"" + strServiceName + L"\") failed. The named service already exists.");
		return services.add(strServiceName);
	}

	void SCMessageSystem::serviceRemove(const String& strServiceName)
	{
		ErrorIfFalse(services.exists(strServiceName), L"MessageSystem::serviceRemove(\"" + strServiceName + L"\") failed. The named service does not exist.");

		// Remove service from each user's subscription list
		for (size_t i = 0; i < users.getNumber(); i++)
		{
			users.get(i)->_mlstSubscribedServiceNames.remove(strServiceName);
		}

		services.remove(strServiceName);
	}

	bool SCMessageSystem::serviceExists(const String& strServiceName)
	{
		return services.exists(strServiceName);
	}

	CMessageService* SCMessageSystem::serviceGet(const String& strServiceName)
	{
		ErrorIfFalse(services.exists(strServiceName), L"MessageSystem::serviceGet(\"" + strServiceName + L"\") failed. The named service does not exist.");
		return services.get(strServiceName);
	}

	CMessageUser* SCMessageSystem::userAdd(const String& strUserName)
	{
		ErrorIfTrue(users.exists(strUserName), L"MessageSystem::userAdd(\"" + strUserName + L"\") failed. The named user already exists.");
		CMessageUser* pUser = users.add(strUserName);
		pUser->_mstrName = strUserName;
		return pUser;
	}

	void SCMessageSystem::userRemove(const String& strUserName)
	{
		ErrorIfFalse(users.exists(strUserName), L"MessageSystem::userRemove(\"" + strUserName + L"\") failed. The named user does not exist.");

		// Remove user from each service's subscription list
		for (size_t i = 0; i < services.getNumber(); i++)
		{
			services.get(i)->_mlstSubscribedUserNames.remove(strUserName);
		}

		users.remove(strUserName);
	}

	bool SCMessageSystem::userExists(const String& strUserName)
	{
		return users.exists(strUserName);
	}

	CMessageUser* SCMessageSystem::userGet(const String& strUserName)
	{
		ErrorIfFalse(users.exists(strUserName), L"MessageSystem::userGet(\"" + strUserName + L"\") failed. The named user does not exist.");
		return users.get(strUserName);
	}

	void SCMessageSystem::subscribeUserToService(const String& strUserName, const String& strServiceName)
	{
		// Make sure both the user and service exist
		ErrorIfFalse(users.exists(strUserName), L"MessageSystem::subscribeUserToService(\"" + strUserName + L", " + strServiceName + L"\") failed. The named user does not exist.");
		ErrorIfFalse(services.exists(strServiceName), L"MessageSystem::subscribeUserToService(\"" + strUserName + L", " + strServiceName + L"\") failed. The named service does not exist.");

		// Add service to the user's subscribed service list
		CMessageUser* pUser = users.get(strUserName);
		if (!pUser->isSubscribedToService(strServiceName))
			pUser->_mlstSubscribedServiceNames.push_back(strServiceName);

		// Add the user to the service's subscribed user list
		CMessageService* pService = services.get(strServiceName);
		if (!pService->isSubscribedToService(strUserName))
			pService->_mlstSubscribedUserNames.push_back(strUserName);
	}

	void SCMessageSystem::unsubscribeUserFromService(const String& strUserName, const String& strServiceName)
	{
		// Make sure both the user and service exist
		ErrorIfFalse(users.exists(strUserName), L"MessageSystem::unsubscribeUserFromService(\"" + strUserName + L", " + strServiceName + L"\") failed. The named user does not exist.");
		ErrorIfFalse(services.exists(strServiceName), L"MessageSystem::unsubscribeUserFromService(\"" + strUserName + L", " + strServiceName + L"\") failed. The named service does not exist.");

		// Remove service from the user's subscribed service list
		CMessageUser* pUser = users.get(strUserName);
		if (pUser->isSubscribedToService(strServiceName))
			pUser->_mlstSubscribedServiceNames.remove(strServiceName);

		// Remove the user from the service's subscribed user list
		CMessageService* pService = services.get(strServiceName);
		if (pService->isSubscribedToService(strUserName))
			pService->_mlstSubscribedUserNames.remove(strUserName);
	}
}