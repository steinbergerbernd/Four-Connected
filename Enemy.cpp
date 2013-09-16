#include "Enemy.h"

Enemy::Enemy(World* world, int horizon, Board* board) :
	GameObject(world), horizon(horizon), board(board)
{
}

Enemy::~Enemy(void)
{
}

// rekursive Methode, um die m�glichen Z�ge f�r die KI zu bewerten
Enemy::turn Enemy::tryMove(bool enemy, int depth, float alpha, float beta)
{
	// neuen Zug anlegen
	struct Enemy::turn currentTurn = {-1, alpha};
	float currentValue;

	// initialisieren des Spielsteines f�r Gegner oder Spieler
	Board::FieldState fieldState;
	if(enemy)
		fieldState = Board::white;
	else
		fieldState = Board::red;

	// Alle m�glichen Kinder (Einwurf-Slots) durchlaufen
	for(int i = 0; i < board->getBoardSize().width; ++i)
	{
		// Wenn der Slot noch frei ist
		if(board->isFreeColumn(i))
		{
			// Spielstein in den aktuellen Slot einwerfen und Spielsituation bewerten
			// Die Bewertung wird durch die Tiefe dividiert, damit Z�ge weiter unten schlechter bewertet werden
			currentValue = board->checkWinningSituation(board->insertToken(i, fieldState), i, fieldState) / (float)(horizon-depth+1);

			// Wenn keine Gewinnsituation aufgetreten ist (kein Blattknoten), dann rekursiv den n�chsten Zug bewerten
			if(currentValue < board->getWinningHeuristic() / (float)(horizon-depth+1) && depth-1 > 0)
				currentValue = tryMove(!enemy, depth-1, -beta, -currentTurn.value).value;

			// Spielstein wieder entfernen
			board->removeToken(i);

			// Wenn der Wert gr��er oder gleich dem Beta-Wert ist, kann ein Cut durchgef�hrt werden
			if(currentValue >= beta)
			{
				currentTurn.value = beta;
				currentTurn.columnIndex = i;
				break;
			}

			// Wenn der aktuelle Zug einen gr��eren Wert als der bisherige hat, dann diesen speichern
			if(currentValue > currentTurn.value)
			{
				currentTurn.value = currentValue;
				currentTurn.columnIndex = i;
			}
		}
	}

	// Bewertung invertieren f�r �bergeordnete Ebene (NegaMax)
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