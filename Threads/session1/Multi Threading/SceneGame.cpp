#include "SceneGame.h"
#include "ModuleGameObject.h"
#include "ModuleResources.h"
#include "Application.h"
#include "Input.h"
#include "Time.h"


void SceneGame::enable()
{
	auto gameObject = new GameObject;
	gameObject->texture = App->modResources->space;

	gameObject = new GameObject;
	gameObject->texture = App->modResources->asteroid1;
	gameObject->x = -200;
	gameObject->y = -100;
	gameObject->width = 200;
	gameObject->height = 200;
	asteroid1 = gameObject;

	gameObject = new GameObject;
	gameObject->texture = App->modResources->asteroid2;
	gameObject->x = 300;
	gameObject->y = 200;
	gameObject->width = 200;
	gameObject->height = 200;

	gameObject = new GameObject;
	gameObject->texture = App->modResources->spacecraft1;
	gameObject->x = 0;
	gameObject->y = 0;
	gameObject->width = 200;
	gameObject->height = 200;
	player = gameObject;
}

void SceneGame::update()
{
	static float move_x = 0.0f;
	static float move_y = 0.0f;

	float thumb_x = Input.leftThumbX;
	float thumb_y = -Input.leftThumbY;
	bool usingThumbStick = abs(thumb_x) > FLT_EPSILON || abs(thumb_y) > FLT_EPSILON;
	
	if (usingThumbStick)
	{
		move_x = thumb_x;
		move_y = thumb_y;
	}
	else
	{
		move_x += (int)(Input.moveRight) * 0.05f;
		move_x -= (int)(Input.moveLeft) * 0.05f;
		move_y += (int)(Input.moveDown) * 0.05f;
		move_y -= (int)(Input.moveUp) * 0.05f;
		if (move_x > 1.0f)  move_x = 1.0f;
		if (move_x < -1.0f) move_x = -1.0f;
		if (move_y > 1.0f)  move_y = 1.0f;
		if (move_y < -1.0f) move_y = -1.0f;
	}
	
	if (abs(move_x) > FLT_EPSILON || abs(move_y) > FLT_EPSILON)
	{
		const float speed = 300.0f;
		player->y += move_y * speed * Time.deltaTime;
		player->x += move_x * speed * Time.deltaTime;
		float new_angle = 180.0f * atan2(move_x, -move_y) / 3.14159f;
		while (new_angle - player->angle > 180.0f) new_angle -= 360.0f;
		while (player->angle - new_angle > 180.0f) new_angle += 360.0f;
		player->angle = 0.8f * player->angle + 0.2f * new_angle;

		if (!usingThumbStick)
		{
			move_x *= 0.9f;
			move_y *= 0.9f;
		}
	}
	else
	{
		move_x = 0.0f;
		move_y = 0.0f;
	}
}

void SceneGame::disable()
{
}
