#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool update() override;

	Behaviour	* addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject);
	Spaceship	* addSpaceship(GameObject *parentGameObject);
	Laser		* addLaser(GameObject *parentGameObject);
	PowerUp     * addPowerUp(GameObject *parentGameObject);

	std::vector<vec2> GetSpaceshipsPosition(const GameObject* ignore) const;

private:

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship spaceships[MAX_CLIENTS];
	Laser lasers[MAX_GAME_OBJECTS];
	PowerUp powerups[MAX_GAME_OBJECTS/2];
};

