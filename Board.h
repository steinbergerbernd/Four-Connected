#pragma once

#include <ClanLib/core.h>
#include <vector>

#include "gameobject.h"

using namespace std;

class Board : public GameObject
{
public:
	Board(World* world, CL_Size boardSize, CL_Rect viewPort);
	~Board(void);

	// Status für ein Feld im Brett (kein Stein, roter Stein oder weißer Stein)
	enum FieldState { none, red, white };
	// mögliche Spielstati (rot gewinnt, unentschieden, weiß gewinnnt oder Spiel läuft noch)
	enum GameState { redWon, tie, whiteWon, running };

	void draw();
	bool update(float elapsed);

	bool isFreeColumn(int columnIndex);

	int insertToken(int posX, int posY, FieldState newFieldState);
	int insertToken(int columnIndex, FieldState newFieldState);

	void removeToken(int columnIndex);

	int getUpperRowIndex(int columnIndex);

	int checkWinningSituation(int rowIndex, int columnIndex, Board::FieldState fieldState);

	Board::GameState getGameState();

	float getWinningHeuristic();

	void updateGameState();

	const vector<vector<Board::FieldState>>& getFields() const;

	CL_Size getBoardSize();
private:
	// speichert das Spielfeld
	vector<vector<FieldState>> fields;

	// bereits gespielte Felder
	int occupiedFields;
	// gesamte Feldanzahl
	int fieldCount;

	// Indizes des letzten Spielzuges
	int lastRowIndex;
	int lastColumnIndex;
	// zuletzt eingeworfener Spielstein
	Board::FieldState lastFieldState;

	// aktueller Spielstatus
	Board::GameState gameState;

	// Größen des Spielbretts und dessen Felder
	CL_Size boardSize;
	CL_Sizef fieldSize;
	CL_Rect viewPort;
};

