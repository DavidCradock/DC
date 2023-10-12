#include "messaging.h"

namespace DC
{
	SCMessageSystem::SCMessageSystem()
	{
	}

	MessageService* SCMessageSystem::serviceAdd(const String& serviceName)
	{
		ErrorIfTrue(services.exists(serviceName), L"MessageSystem::serviceAdd(\"" + serviceName + L"\") failed. The named service already exists.");
		return services.add(serviceName);
	}

	void SCMessageSystem::serviceRemove(const String& serviceName)
	{
		ErrorIfFalse(services.exists(serviceName), L"MessageSystem::serviceRemove(\"" + serviceName + L"\") failed. The named service does not exist.");

		// Remove service from each user's subscription list
		for (size_t i = 0; i < users.getNumber(); i++)
		{
			users.get(i)->subscribedServiceNames.remove(serviceName);
		}

		services.remove(serviceName);
	}

	bool SCMessageSystem::serviceExists(const String& serviceName)
	{
		return services.exists(serviceName);
	}

	MessageService* SCMessageSystem::serviceGet(const String& serviceName)
	{
		ErrorIfFalse(services.exists(serviceName), L"MessageSystem::serviceGet(\"" + serviceName + L"\") failed. The named service does not exist.");
		return services.get(serviceName);
	}

	MessageUser* SCMessageSystem::userAdd(const String& userName)
	{
		ErrorIfTrue(users.exists(userName), L"MessageSystem::userAdd(\"" + userName + L"\") failed. The named user already exists.");
		MessageUser* pUser = users.add(userName);
		pUser->name = userName;
		return pUser;
	}

	void SCMessageSystem::userRemove(const String& userName)
	{
		ErrorIfFalse(users.exists(userName), L"MessageSystem::userRemove(\"" + userName + L"\") failed. The named user does not exist.");

		// Remove user from each service's subscription list
		for (size_t i = 0; i < services.getNumber(); i++)
		{
			services.get(i)->subscribedUserNames.remove(userName);
		}

		users.remove(userName);
	}

	bool SCMessageSystem::userExists(const String& userName)
	{
		return users.exists(userName);
	}

	MessageUser* SCMessageSystem::userGet(const String& userName)
	{
		ErrorIfFalse(users.exists(userName), L"MessageSystem::userGet(\"" + userName + L"\") failed. The named user does not exist.");
		return users.get(userName);
	}

	void SCMessageSystem::subscribeUserToService(const String& userName, const String& serviceName)
	{
		// Make sure both the user and service exist
		ErrorIfFalse(users.exists(userName), L"MessageSystem::subscribeUserToService(\"" + userName + L", " + serviceName + L"\") failed. The named user does not exist.");
		ErrorIfFalse(services.exists(serviceName), L"MessageSystem::subscribeUserToService(\"" + userName + L", " + serviceName + L"\") failed. The named service does not exist.");

		// Add service to the user's subscribed service list
		MessageUser* pUser = users.get(userName);
		if (!pUser->isSubscribedToService(serviceName))
			pUser->subscribedServiceNames.push_back(serviceName);

		// Add the user to the service's subscribed user list
		MessageService* pService = services.get(serviceName);
		if (!pService->isSubscribedToService(userName))
			pService->subscribedUserNames.push_back(userName);
	}

	void SCMessageSystem::unsubscribeUserFromService(const String& userName, const String& serviceName)
	{
		// Make sure both the user and service exist
		ErrorIfFalse(users.exists(userName), L"MessageSystem::unsubscribeUserFromService(\"" + userName + L", " + serviceName + L"\") failed. The named user does not exist.");
		ErrorIfFalse(services.exists(serviceName), L"MessageSystem::unsubscribeUserFromService(\"" + userName + L", " + serviceName + L"\") failed. The named service does not exist.");

		// Remove service from the user's subscribed service list
		MessageUser* pUser = users.get(userName);
		if (pUser->isSubscribedToService(serviceName))
			pUser->subscribedServiceNames.remove(serviceName);

		// Remove the user from the service's subscribed user list
		MessageService* pService = services.get(serviceName);
		if (pService->isSubscribedToService(userName))
			pService->subscribedUserNames.remove(userName);
	}
}