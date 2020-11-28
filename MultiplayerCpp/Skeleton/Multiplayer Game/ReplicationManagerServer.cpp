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
	packet << ClientMessage::Input;
	packet << actions.size();

	for (auto item = actions.begin(); item != actions.end();) {
	
		packet << (*item).first;
		packet << (*item).second;

		// Clear/Update states
		if ((*item).second == ReplicationAction::Destroy) {
			item = actions.erase(item);
			continue;
		}
		else if ((*item).second != ReplicationAction::None) {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject((*item).first);

			// TODO: save all GO fields
			packet << gameObject->position.x;
			packet << gameObject->position.y;
			packet << gameObject->size.x;
			packet << gameObject->size.y;
			packet << gameObject->angle;
			packet << gameObject->tag;
			packet << gameObject->networkInterpolationEnabled;
			packet << gameObject->state;

			if ((*item).second == ReplicationAction::Create) {
				// TODO: write sprite bla bla??
				if (gameObject->sprite != nullptr && gameObject->sprite->texture != nullptr) {
					packet << gameObject->sprite->texture->id;
					packet.Write(gameObject->sprite->color);
					packet << gameObject->sprite->order;
					packet.Write(gameObject->sprite->pivot);
				}
				else {
					packet << -1;
				}

				if (gameObject->behaviour != nullptr) {
					packet << gameObject->behaviour->type();
					gameObject->behaviour->write(packet);
				}
				else {
					packet << BehaviourType::None;
				}
			}

			(*item).second = ReplicationAction::None;
		}
		++item;
	}
}