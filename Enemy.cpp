#include "Enemy.h"

Enemy::Enemy(World* world, int horizon, Board* board) :
	GameObject(world), horizon(horizon), board(board)
{
}

Enemy::~Enemy(void)
{
}

// rekursive Methode, um die möglichen Züge für die KI zu bewerten
Enemy::turn Enemy::tryMove(bool enemy, int depth, float alpha, float beta)
{
	// neuen Zug anlegen
	struct Enemy::turn currentTurn = {-1, alpha};
	float currentValue;

	// initialisieren des Spielsteines für Gegner oder Spieler
	Board::FieldState fieldState;
	if(enemy)
		fieldState = Board::white;
	else
		fieldState = Board::red;

	// Alle möglichen Kinder (Einwurf-Slots) durchlaufen
	for(int i = 0; i < board->getBoardSize().width; ++i)
	{
		// Wenn der Slot noch frei ist
		if(board->isFreeColumn(i))
		{
			// Spielstein in den aktuellen Slot einwerfen und Spielsituation bewerten
			// Die Bewertung wird durch die Tiefe dividiert, damit Züge weiter unten schlechter bewertet werden
			currentValue = board->checkWinningSituation(board->insertToken(i, fieldState), i, fieldState) / (float)(horizon-depth+1);

			// Wenn keine Gewinnsituation aufgetreten ist (kein Blattknoten), dann rekursiv den nächsten Zug bewerten
			if(currentValue < board->getWinningHeuristic() / (float)(horizon-depth+1) && depth-1 > 0)
				currentValue = tryMove(!enemy, depth-1, -beta, -currentTurn.value).value;

			// Spielstein wieder entfernen
			board->removeToken(i);

			// Wenn der Wert größer oder gleich dem Beta-Wert ist, kann ein Cut durchgeführt werden
			if(currentValue >= beta)
			{
				currentTurn.value = beta;
				currentTurn.columnIndex = i;
				break;
			}

			// Wenn der aktuelle Zug einen größeren Wert als der bisherige hat, dann diesen speichern
			if(currentValue > currentTurn.value)
			{
				currentTurn.value = currentValue;
				currentTurn.columnIndex = i;
			}
		}
	}

	// Bewertung invertieren für übergeordnete Ebene (NegaMax)
	currentTurn.value *= -1;

	return currentTurn;
}

// Ein kompletter Zug eines Gegners
bool Enemy::move()
{
	// Besten Zug finden
	struct Enemy::turn currentTurn = tryMove(true, horizon, -10000, 10000);

	// Stein einwerfen
	board->insertToken(currentTurn.columnIndex, Board::white);

	return true;
}