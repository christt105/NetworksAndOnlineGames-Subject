#pragma once


enum class BehaviourType : uint8;

struct Behaviour
{
	GameObject *gameObject = nullptr;
	bool isServer = false;
	bool isLocalPlayer = false;

	virtual ~Behaviour() {};

	virtual BehaviourType type() const = 0;

	virtual void start() { }

	virtual void onInput(const InputController &input) { }

	virtual void update() { }

	virtual void destroy() { }

	virtual void onCollisionTriggered(Collider &c1, Collider &c2) { }

	virtual void write(OutputMemoryStream &packet) { }

	virtual void read(const InputMemoryStream &packet) { }
};


enum class BehaviourType : uint8
{
	None,
	Spaceship,
	Laser,
	PowerUp
};


struct Laser : public Behaviour
{
	float secondsSinceCreation = 0.0f;

	BehaviourType type() const override { return BehaviourType::Laser; }

	void start() override;

	void write(OutputMemoryStream& packet) override;
	void read(const InputMemoryStream& packet) override;

	void update() override;
};

enum PowerUpType {
	None,
	Double,
	BackAndFront,
	Shield,
	Back
};

struct PowerUp : public Behaviour
{
	float secondsSinceCreation = 0.0f;

	PowerUpType pwt = None;

	BehaviourType type() const override { return BehaviourType::PowerUp; }

	void start() override;

	void update() override;

	void write(OutputMemoryStream& packet) override;

	void read(const InputMemoryStream& packet) override;
};


struct Spaceship : public Behaviour
{
	static const uint8 MAX_HIT_POINTS = 5;
	uint8 hitPoints = MAX_HIT_POINTS;

	PowerUpType pwt = None;

	float angle = 0.f;
	float orbit_speed = 2.5f;
	float radius = 90.f;

	GameObject *lifebar = nullptr;
	GameObject *shield = nullptr;

	BehaviourType type() const override { return BehaviourType::Spaceship; }

	void start() override;

	void onInput(const InputController &input) override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider &c1, Collider &c2) override;

	void write(OutputMemoryStream &packet) override;

	void read(const InputMemoryStream &packet) override;
};
