#pragma once

#include "gameobject.h"
#include "State.h"
#include "Board.h"

class Enemy : public GameObject
{
public:
	Enemy(World* world, int horizon, Board* board);
	virtual ~Enemy(void);

	bool move();
private:
	// Struktur um die Bewertung und den Spaltenindex eines Zuges zu speichern
	struct turn
	{
		int columnIndex;
		float value;
	};

	turn tryMove(bool playerStarts, int depth, float alpha, float beta);

	// Suchtiefe
	int horizon;

	// Zeiger auf das Spielbrett
	Board* board;
};

