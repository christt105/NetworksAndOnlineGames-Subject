#include "Networks.h"
#include "ModuleLinkingContext.h"


void ModuleLinkingContext::registerNetworkGameObject(GameObject* gameObject)
{
	gameObject->networkId = makeNetworkId(networkGameObjectsMap.size());
	networkGameObjectsMap[gameObject->networkId] = gameObject;
}

void ModuleLinkingContext::registerNetworkGameObjectWithNetworkId(GameObject * gameObject, uint32 networkId)
{
	ASSERT(networkId != 0);
	ASSERT(networkGameObjectsMap[networkId] == nullptr);
	networkGameObjectsMap[networkId] = gameObject;
	gameObject->networkId = networkId;
}

GameObject * ModuleLinkingContext::getNetworkGameObject(uint32 networkId, bool safeNetworkIdCheck)
{
	auto it = networkGameObjectsMap.find(networkId);
	return (it != networkGameObjectsMap.end()) ? (*it).second : nullptr;
}

void ModuleLinkingContext::getNetworkGameObjects(GameObject * gameObjects[MAX_NETWORK_OBJECTS], uint16 * count)
{
	uint16 c = 0;

	for (auto i = networkGameObjectsMap.begin(); i != networkGameObjectsMap.end() && c < MAX_NETWORK_OBJECTS; ++i)
	{
		gameObjects[c++] = (*i).second;
	}

	*count = c;
}

uint16 ModuleLinkingContext::getNetworkGameObjectsCount() const
{
	return networkGameObjectsMap.size();
}

void ModuleLinkingContext::unregisterNetworkGameObject(GameObject *gameObject)
{
	networkGameObjectsMap.erase(gameObject->networkId);
	gameObject->networkId = 0;
}

void ModuleLinkingContext::clear()
{
	networkGameObjectsMap.clear();
}

uint32 ModuleLinkingContext::makeNetworkId(uint16 arrayIndex)
{
	ASSERT(arrayIndex < MAX_NETWORK_OBJECTS);
	uint32 magicNumber = nextMagicNumber++;
	uint32 networkId = (magicNumber << 16) | arrayIndex;
	return networkId;
}
