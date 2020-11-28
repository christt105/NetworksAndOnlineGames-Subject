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
			packet >> gameObject->state;

			if (action == ReplicationAction::Create) {
				// TODO: read sprite bla bla??
				int id = -1;
				packet >> id;
				if (id != -1) {
					gameObject->sprite = App->modRender->addSprite(gameObject);
					gameObject->sprite->texture = App->modTextures->GetTextureByID(id);
					ASSERT(gameObject->sprite->texture != nullptr);
					//packet.Read(&gameObject->sprite->color, sizeof(float) * 4);
					//packet >> gameObject->sprite->order;
					//packet.Read(&gameObject->sprite->pivot, sizeof(float) * 2);
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
					if (gameObject->behaviour != nullptr)
						gameObject->behaviour->read(packet);
				}
			}

			LOG("POSX: %f", gameObject->position.x);
			LOG("POSY: %f", gameObject->position.y);
			LOG("TAG %i", (int)gameObject->tag); // haha it works!!
		}

	}
}
