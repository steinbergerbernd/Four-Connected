#pragma once

class World;

class GameObject
{
public:
	GameObject(World* world);
	virtual ~GameObject(void);

	virtual void draw() {};
	virtual bool update(float elapsed) { return false; }

protected:
	World* world;
};

