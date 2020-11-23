#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session

void ReplicationManagerServer::create(uint32 networkId)
{
	actions.emplace(networkId, ReplicationAction::Create);
}

void ReplicationManagerServer::update(uint32 networkId)
{
	actions[networkId] = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	actions[networkId] = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{
	packet << PROTOCOL_ID;

	for (auto item = actions.begin(); item != actions.end();) {
	
		packet << (*item).first;
		packet << (*item).second;

		// Clear/Update states
		if ((*item).second != ReplicationAction::Destroy) {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject((*item).first);

			// TODO: save all GO fields

			(*item).second = ReplicationAction::None;
			++item;
		}
		else {
			item = actions.erase(item);
		}
	}
}
