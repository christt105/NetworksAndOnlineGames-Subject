#include "Networks.h"
#include "Behaviours.h"



void Laser::start()
{
	gameObject->networkInterpolationEnabled = false;

	App->modSound->playAudioClip(App->modResources->audioClipLaser);
}

void Laser::write(OutputMemoryStream& packet)
{
	packet << secondsSinceCreation;
}

void Laser::read(const InputMemoryStream& packet)
{
	packet >> secondsSinceCreation;
}

void Laser::update()
{
	secondsSinceCreation += Time.deltaTime;

	const float pixelsPerSecond = 1000.0f;
	gameObject->position += vec2FromDegrees(gameObject->angle) * pixelsPerSecond * Time.deltaTime;

	if (isServer)
	{
		const float neutralTimeSeconds = 0.1f;
		if (secondsSinceCreation > neutralTimeSeconds && gameObject->collider == nullptr) {
			gameObject->collider = App->modCollision->addCollider(ColliderType::Laser, gameObject);
		}

		const float lifetimeSeconds = 2.0f;
		if (secondsSinceCreation >= lifetimeSeconds) {
			NetworkDestroy(gameObject);
		}
	}
}





void Spaceship::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);

	lifebar = Instantiate();
	lifebar->sprite = App->modRender->addSprite(lifebar);
	lifebar->sprite->pivot = vec2{ 0.0f, 0.5f };
	lifebar->sprite->order = 5;

	if (shield == nullptr) {
		shield = Instantiate();
		shield->sprite = App->modRender->addSprite(shield);
		shield->sprite->texture = App->modResources->shield;
	}

	if (!isServer && App->modLinkingContext->getNetworkGameObject(App->modNetClient->GetNetworkID()) == gameObject) {
		auto spaceships = App->modBehaviour->GetSpaceshipsPosition(gameObject);
		for (int i = 0; i < spaceships.size(); ++i) {
			GameObject* arr = Instantiate();
			arr->sprite = App->modRender->addSprite(arr);
			arr->sprite->texture = App->modResources->arrow;
			arr->sprite->color = vec4{ (float)(rand()%255) / 255.f,(float)(rand() % 255) / 255.f,(float)(rand() % 255) / 255.f,1.f };
			arrows.push_back(arr);
		}
	}
}

void Spaceship::onInput(const InputController& input)
{
	if (input.horizontalAxis != 0.0f)
	{
		const float rotateSpeed = 180.0f;
		gameObject->angle += input.horizontalAxis * rotateSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionDown == ButtonState::Pressed)
	{
		const float advanceSpeed = 200.0f;
		gameObject->position += vec2FromDegrees(gameObject->angle) * advanceSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionLeft == ButtonState::Press)
	{
		if (isServer)
		{
			switch (pwt)
			{
			case PowerUpType::Double: {
				float angles[2] = { -15.f, 15.f };
				for (int i = 0; i < 2; ++i) {
					GameObject* laser = NetworkInstantiate();

					laser->position = gameObject->position;
					laser->angle = gameObject->angle + angles[i];
					laser->size = { 20, 60 };

					laser->sprite = App->modRender->addSprite(laser);
					laser->sprite->order = 3;
					laser->sprite->texture = App->modResources->laser;
					laser->sprite->color = { 1.f, 0.623f, 0.2f, 1.f };

					Laser* laserBehaviour = App->modBehaviour->addLaser(laser);
					laserBehaviour->isServer = isServer;

					laser->tag = gameObject->tag;
				}
				break;
			}
			case PowerUpType::BackAndFront: {
				float angles[2] = { 0.f, 180.f };
				for (int i = 0; i < 2; ++i) {
					GameObject* laser = NetworkInstantiate();

					laser->position = gameObject->position;
					laser->angle = gameObject->angle + angles[i];
					laser->size = { 20, 60 };

					laser->sprite = App->modRender->addSprite(laser);
					laser->sprite->order = 3;
					laser->sprite->texture = App->modResources->laser;
					laser->sprite->color = { 0.f, 1.f, 0.f, 1.f };

					Laser* laserBehaviour = App->modBehaviour->addLaser(laser);
					laserBehaviour->isServer = isServer;

					laser->tag = gameObject->tag;
				}
				break;
			}
			case PowerUpType::Back: {
				float angles[3] = { (180.f + 15.f), 0.f, (180.f - 15.f) };
				for (int i = 0; i < 3; ++i) {
					GameObject* laser = NetworkInstantiate();

					laser->position = gameObject->position;
					laser->angle = gameObject->angle + angles[i];
					laser->size = { 20, 60 };

					laser->sprite = App->modRender->addSprite(laser);
					laser->sprite->order = 3;
					laser->sprite->texture = App->modResources->laser;
					laser->sprite->color = { 0.f, 0.f, 1.f, 1.f };

					Laser* laserBehaviour = App->modBehaviour->addLaser(laser);
					laserBehaviour->isServer = isServer;

					laser->tag = gameObject->tag;
				}
				break;
			}
			default:
				GameObject* laser = NetworkInstantiate();

				laser->position = gameObject->position;
				laser->angle = gameObject->angle;
				laser->size = { 20, 60 };

				laser->sprite = App->modRender->addSprite(laser);
				laser->sprite->order = 3;
				laser->sprite->texture = App->modResources->laser;
				laser->sprite->color = { 1.f, 0.f, 0.f, 1.f };

				Laser* laserBehaviour = App->modBehaviour->addLaser(laser);
				laserBehaviour->isServer = isServer;

				laser->tag = gameObject->tag;
				break;
			}
		}
	}
}

void Spaceship::update()
{
	static const vec4 colorAlive = vec4{ 0.2f, 1.0f, 0.1f, 0.5f };
	static const vec4 colorDead = vec4{ 1.0f, 0.2f, 0.1f, 0.5f };
	const float lifeRatio = max(0.01f, (float)(hitPoints) / (MAX_HIT_POINTS));
	lifebar->position = gameObject->position + vec2{ -50.0f, -50.0f };
	lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
	lifebar->sprite->color = lerp(colorDead, colorAlive, lifeRatio);

	if (pwt != PowerUpType::Shield && shield != nullptr) {
		Destroy(shield);
		shield = nullptr;
	}
	if (shield != nullptr) {
		angle += orbit_speed * Time.deltaTime;
		shield->position = gameObject->position + vec2{ radius * cos(angle), radius * sin(angle) };
	}

	if (!isServer && App->modLinkingContext->getNetworkGameObject(App->modNetClient->GetNetworkID()) == gameObject) {
		auto spaceships = App->modBehaviour->GetSpaceshipsPosition(gameObject);
		if (spaceships.size() > arrows.size()) {
			int n = spaceships.size() - arrows.size();
			for (int a = 0; a < n; ++a) {
				GameObject* arr = Instantiate();
				arr->sprite = App->modRender->addSprite(arr);
				arr->sprite->texture = App->modResources->arrow;
				arr->sprite->color = vec4{ 1.f,0.6f,0.1f,1.f };
				arrows.push_back(arr);
			}
		}
		if (spaceships.size() < arrows.size()) {
			int n = arrows.size() - spaceships.size();
			for (int a = 0; a < n; ++a) {
				Destroy(arrows[a]);
				arrows.pop_back();
			}
		}
		float rad = 75.f;
		for (int i = 0; i < spaceships.size(); ++i) {
			vec2 direction = normalize(spaceships[i] - gameObject->position);
			arrows[i]->position = gameObject->position + direction * rad;
			arrows[i]->angle = degreesFromRadians(atan2f(direction.y, direction.x));
		}
	}
}

void Spaceship::destroy()
{
	Destroy(lifebar);
	if (shield != nullptr) {
		Destroy(shield);
	}
	for (auto i = arrows.begin(); i != arrows.end(); ++i) {
		Destroy(*i);
	}
}

bool Spaceship::checkPosition(const vec2& pos1, const vec2& pos2, float range) const
{
	return pos1.x - range < pos2.x && pos1.y - range < pos2.y && pos1.x + range > pos2.x && pos1.y + range > pos2.y;
}

void Spaceship::onCollisionTriggered(Collider &c1, Collider &c2)
{
	if (c2.type == ColliderType::Laser && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{
			if (pwt == PowerUpType::Shield) {
				pwt = PowerUpType::None;
				Destroy(shield);
				shield = nullptr;
				NetworkDestroy(c2.gameObject); // Destroy the laser
				NetworkUpdate(gameObject);
				return;
			}

			NetworkDestroy(c2.gameObject); // Destroy the laser
		
			if (hitPoints > 0)
			{
				hitPoints--;
				NetworkUpdate(gameObject);
			}

			float size = 30 + 50.0f * Random.next();
			vec2 position = gameObject->position + 50.0f * vec2{Random.next() - 0.5f, Random.next() - 0.5f};

			if (hitPoints <= 0)
			{
				// Centered big explosion
				size = 250.0f + 100.0f * Random.next();
				position = gameObject->position;

				// Create power up
				auto go = NetworkInstantiate();
				go->position = gameObject->position;

				go->sprite = App->modRender->addSprite(go);
				go->behaviour = App->modBehaviour->addPowerUp(go);
				switch (rand() % 4) {
				case 0: {
					go->sprite->texture = App->modResources->power_up1;
					((PowerUp*)go->behaviour)->pwt = PowerUpType::Back;
					break; }
				case 1: {
					go->sprite->texture = App->modResources->power_up2;
					((PowerUp*)go->behaviour)->pwt = PowerUpType::BackAndFront;
					break; }
				case 2: {
					go->sprite->texture = App->modResources->power_up3;
					((PowerUp*)go->behaviour)->pwt = PowerUpType::Double;
					break; }
				case 3: {
					go->sprite->texture = App->modResources->power_up4;
					((PowerUp*)go->behaviour)->pwt = PowerUpType::Shield;
					break; }
				}
				go->collider = App->modCollision->addCollider(ColliderType::PowerUp, go);


				go->behaviour->isServer = true;

				NetworkDestroy(gameObject);
			}

			GameObject *explosion = NetworkInstantiate();
			explosion->position = position;
			explosion->size = vec2{ size, size };
			explosion->angle = 365.0f * Random.next();

			explosion->sprite = App->modRender->addSprite(explosion);
			explosion->sprite->texture = App->modResources->explosion1;
			explosion->sprite->order = 100;

			explosion->animation = App->modRender->addAnimation(explosion);
			explosion->animation->clip = App->modResources->explosionClip;

			NetworkDestroy(explosion, 2.0f);

			App->modNetServer->playAudio(App->modResources->audioClipExplosion->id);
		}
	}
	if (c2.type == ColliderType::PowerUp) {
		if (pwt == PowerUpType::Shield) {
			if (shield != nullptr)
				Destroy(shield);
			shield = nullptr;
		}
		pwt = ((PowerUp*)c2.gameObject->behaviour)->pwt;
		NetworkDestroy(c2.gameObject);
	}
}


void Spaceship::write(OutputMemoryStream& packet)
{
	packet << hitPoints;
	packet << pwt;
	if (pwt == PowerUpType::Shield) {
		if (shield == nullptr) {
			shield = Instantiate();
			shield->sprite = App->modRender->addSprite(shield);
			shield->sprite->texture = App->modResources->shield;
		}
		packet << shield->position.x;
		packet << shield->position.y;
	}
	else if (shield != nullptr) {
		Destroy(shield);
		shield = nullptr;
	}
}

void Spaceship::read(const InputMemoryStream& packet)
{
	packet >> hitPoints;
	packet >> pwt;
	if (pwt == PowerUpType::Shield) {
		if (shield == nullptr) {
			shield = Instantiate();
			shield->sprite = App->modRender->addSprite(shield);
			shield->sprite->texture = App->modResources->shield;
		}
		packet >> shield->position.x;
		packet >> shield->position.y;
	}
	else if (shield != nullptr) {
		Destroy(shield);
		shield = nullptr;
	}
}

void PowerUp::start()
{

}

void PowerUp::update()
{
	gameObject->angle += 0.1f;
}

void PowerUp::write(OutputMemoryStream& packet)
{
	packet << pwt;
}

void PowerUp::read(const InputMemoryStream& packet)
{
	packet >> pwt;

	//switch (pwt) {
	//case PowerUpType::Back: {
	//	gameObject->sprite->texture = App->modResources->power_up1;
	//	break; }
	//case PowerUpType::BackAndFront: {
	//	gameObject->sprite->texture = App->modResources->power_up2;
	//	break; }
	//case PowerUpType::Double: {
	//	gameObject->sprite->texture = App->modResources->power_up3;
	//	break; }
	//case PowerUpType::Shield: {
	//	gameObject->sprite->texture = App->modResources->power_up4;
	//	break; }
	//}
}
