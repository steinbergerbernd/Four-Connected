#pragma once

#include "GameObject.h"
#include "Board.h"
#include "Enemy.h"

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include <list>

class World
{
public:
	World(CL_DisplayWindow &display_window);
	~World(void);

	CL_ResourceManager resources;
	CL_GraphicContext get_gc() { return graphicContext; }

	void run();
private:
	CL_DisplayWindow window;
	CL_GraphicContext graphicContext;

	//Spielbrett
	Board* board;
	//KI für den Gegner
	Enemy* enemy;

	std::list<GameObject*> objects;

	//Kennzeichen ob Spieler bereits gezogen hat
	bool playerMoved;
	//Kennzeichen ob der Spielstein des Spielers bereits gezeichnet wurde
	bool playerDrawn;

	void onMouseDown(const CL_InputEvent &key, const CL_InputState &state);

	void addObject(GameObject* object);

	float getElapsedTime();

	void draw();
	void update();
};

