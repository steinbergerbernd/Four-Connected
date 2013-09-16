#include "World.h"

#include <ClanLib/core.h>

//Spielbrettdimensionen
const int boardWidth = 7;
const int boardHeight = 6;
//Suchtiefe für die KI
const int searchDepth = 11;

World::World(CL_DisplayWindow &displayWindow) : window(displayWindow)
{
	graphicContext = window.get_gc();
	
	// Kennzeichen ob Spieler oder Gegner beginnt
	playerMoved = false;
	playerDrawn = false;

	// Initialisierung der Objekte
	board = new Board(this, CL_Size(boardWidth, boardHeight), window.get_viewport());
	enemy = new Enemy(this, searchDepth, board);

	addObject(board);
}

World::~World(void)
{
	// Löschen aller GameObjects
	std::list<GameObject*>::iterator it;
	for(it = objects.begin(); it != objects.end(); ++it)
		delete (*it);
}

// Objekt in den Container legen
void World::addObject(GameObject *object)
{
	objects.push_back(object);
}

void World::run(void)
{
	CL_Slot slotMouseDown = window.get_ic().get_mouse().sig_key_down().connect(this, &World::onMouseDown);

	//Bei Escape Schleife abbrechen und Programm beenden
	while (!window.get_ic().get_keyboard().get_keycode(CL_KEY_ESCAPE))
	{
		update();
		draw();

		window.flip(1);
		CL_KeepAlive::process();
	}
}

// Vergangene Zeit berechnen
float World::getElapsedTime()
{
	static unsigned int lastTime = 0;

	unsigned int newTime = CL_System::get_time();

	if (lastTime == 0)
		lastTime = newTime;

	int deltaTime = (newTime - lastTime);

	lastTime = newTime;

	return deltaTime * 0.001f;
}

// Event für Mausklick
void World::onMouseDown(const CL_InputEvent &key, const CL_InputState &state)
{
	// Wenn linke Maus geklickt wurde, der Spieler an der Reihe ist und das Spiel noch nicht beendet ist
	if (key.id == CL_MOUSE_LEFT && !playerMoved && board->getGameState() == Board::running)
	{
		// Spielerzug durchführen
		playerMoved = board->insertToken(key.mouse_pos.x, key.mouse_pos.y, Board::red) > -1;
		// Spielstatus aktualisieren
		board->updateGameState();
	}
}

//Updates ausführen
void World::update()
{	
	//Vergangene Zeit berechnen
	float elapsed = getElapsedTime();

	//Update aller GameObjects aufrufen
	std::list<GameObject *>::iterator it;
	for (it = objects.begin(); it != objects.end(); )
	{
		//Liefert ein Objekt "false" zurück, wird dieses gelöscht
		if (!(*it)->update(elapsed))
		{
			delete (*it);
			it = objects.erase(it);
		}
		else
			++it;

		// Wenn der Gegner an der Reihe ist und das Spiel noch läuft
		if(playerMoved && playerDrawn && board->getGameState() == Board::running)
		{
			// Gegnerzug durchführen
			enemy->move();
			// Spielstatus aktualisieren
			board->updateGameState();

			playerMoved = false;
			playerDrawn = false;
		}
	}
}

// Zeichnen aller Komponenten
void World::draw()
{
	CL_Draw::fill(graphicContext, window.get_viewport(), CL_Colorf::black);

	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); ++it)
		(*it)->draw();

	playerDrawn = playerMoved;
}