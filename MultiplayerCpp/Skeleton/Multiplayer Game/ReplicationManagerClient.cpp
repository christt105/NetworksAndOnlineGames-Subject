#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{
	size_t num;
	packet >> num;

	for (int i = 0; i < num; ++i) {
		uint32 id;
		packet >> id;

		ReplicationAction action;
		packet >> action;

		if (action == ReplicationAction::Destroy) {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(id);
			// TODO: destroy it
			App->modLinkingContext->unregisterNetworkGameObject(gameObject);
			App->modGameObject->Destroy(gameObject);
		}
		else if (action != ReplicationAction::None) {
			GameObject* gameObject = nullptr;
			if (action == ReplicationAction::Update) {
				gameObject = App->modLinkingContext->getNetworkGameObject(id);
				if (gameObject == nullptr) {
					gameObject = App->modGameObject->Instantiate();
					App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, id);
				}
			}
			else {
				gameObject = App->modGameObject->Instantiate();
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, id);
			}

			packet >> gameObject->position.x;
			packet >> gameObject->position.y;
			packet >> gameObject->size.x;
			packet >> gameObject->size.y;
			packet >> gameObject->angle;
			packet >> gameObject->tag;
			packet >> gameObject->networkInterpolationEnabled;

			if (action == ReplicationAction::Create) {
				int id = -1;
				packet >> id;
				if (id != -1) {
					gameObject->sprite = App->modRender->addSprite(gameObject);
					gameObject->sprite->texture = App->modTextures->GetTextureByID(id);
					ASSERT(gameObject->sprite->texture != nullptr);
					packet >> gameObject->sprite->color.r;
					packet >> gameObject->sprite->color.g;
					packet >> gameObject->sprite->color.b;
					packet >> gameObject->sprite->color.a;
					packet >> gameObject->sprite->order;
					packet >> gameObject->sprite->pivot.x;
					packet >> gameObject->sprite->pivot.y;
				}

				BehaviourType behaviour = BehaviourType::None;
				packet >> behaviour;
				if (behaviour != BehaviourType::None) {
					switch (behaviour)
					{
					case BehaviourType::Spaceship:
						gameObject->behaviour = App->modBehaviour->addSpaceship(gameObject);
						break;
					case BehaviourType::Laser:
						gameObject->behaviour = App->modBehaviour->addLaser(gameObject);
						break;
					default:
						WLOG("Behaviour with type %i not in switch", behaviour);
						break;
					}
					if (gameObject->behaviour != nullptr) {
						gameObject->behaviour->read(packet);
					}
				}
			}
		}

	}
}
