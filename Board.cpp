#include "Board.h"
#include "World.h"

// Gesamtsumme der Heuristik bei 4 in einer Reihe
const float winningHeuristic = 81.0f;
// Potenz für die Bewertung des Spielzuges
const int bonusPower = 3;

Board::Board(World* world, CL_Size boardSize, CL_Rect viewPort) : GameObject(world), viewPort(viewPort), boardSize(boardSize)
{
	// neues Spielbrett anlegen
	fields = vector<vector<FieldState>>(boardSize.height, vector<FieldState>(boardSize.width, none));

	// Breite und Höhe aufgrund der Fenstergröße errechnen
	float fieldWidth = viewPort.get_width() / (float)boardSize.width;
	float fieldHeight = viewPort.get_height() / (float)boardSize.height;

	// Felder quadratisch setzen
	if(fieldWidth < fieldHeight)
		fieldHeight = fieldWidth;
	else
		fieldWidth = fieldHeight;

	fieldSize = CL_Sizex<float>(fieldWidth, fieldHeight); 

	occupiedFields = 0;
	fieldCount = boardSize.width * boardSize.height;
	gameState = running;

	lastRowIndex = 0;
	lastColumnIndex = 0;
}

Board::~Board(void)
{
}

const vector<vector<Board::FieldState>>& Board::getFields() const
{
	return fields;
}

CL_Size Board::getBoardSize()
{
	return boardSize;
}

Board::GameState Board::getGameState()
{
	return gameState;
}

float Board::getWinningHeuristic()
{
	return winningHeuristic;
}

// Zeichnen des Spielfeldes
void Board::draw()
{
	CL_Rectf fieldRect;
	CL_GraphicContext gc = world->get_gc();

	for(int i = 0; i < boardSize.height; ++i)
	{
		float yPos = (boardSize.height - 1 - i) * fieldSize.height;
		for(int j = 0; j < boardSize.width; ++j)
		{
			float xPos = j * fieldSize.width;
			fieldRect = CL_Rectf(xPos, yPos, fieldSize);

			CL_Draw::box(gc, fieldRect, CL_Colorf::white);

			switch(fields[i][j])
			{
				case none:
					break;
				case red:
					CL_Draw::circle(gc, xPos + fieldSize.width / 2, yPos + fieldSize.height / 2, fieldSize.height / 2 - 1, CL_Colorf::red);
					break;
				case white:
					CL_Draw::circle(gc, xPos + fieldSize.width / 2, yPos + fieldSize.height / 2, fieldSize.height / 2 - 1, CL_Colorf::white);
					break;
				default:
					break;
			}
		}
	}
}

bool Board::update(float elapsed)
{
	return true;
}

// Einwerfen eines Spielsteines über die Mausposition
int Board::insertToken(int posX, int posY, FieldState newFieldState)
{
	if(posX <= fieldSize.width * boardSize.width && posY <= fieldSize.height * boardSize.height)
	{
		int columnIndex = (int)(posX / fieldSize.width);

		for(int i = 0; i < boardSize.height; ++i)
		{
			if(fields[i][columnIndex] == none)
			{
				fields[i][columnIndex] = newFieldState;
				++occupiedFields;
				lastRowIndex = i;
				lastColumnIndex = columnIndex;
				lastFieldState = newFieldState;
				return i;
			}
		}
	}

	return -1;
}

// Einwerfen eines Spielsteines über den Spaltenindex
int Board::insertToken(int columnIndex, FieldState newFieldState)
{
	if(columnIndex < boardSize.width)
	{
		for(int i = 0; i < boardSize.height; ++i)
		{
			if(fields[i][columnIndex] == none)
			{
				fields[i][columnIndex] = newFieldState;
				++occupiedFields;
				lastRowIndex = i;
				lastColumnIndex = columnIndex;
				lastFieldState = newFieldState;
				return i;
			}
		}
	}

	return -1;
}

// Prüfen ob ein Slot frei ist
bool Board::isFreeColumn(int columnIndex)
{
	return fields[boardSize.height-1][columnIndex] == none;
}

// letzten Spielstein bei übergebenem Spaltenindex wieder entfernen
void Board::removeToken(int columnIndex)
{
	if(columnIndex < boardSize.width)
	{
		for(int i = boardSize.height - 1; i >= 0; --i)
		{
			if(fields[i][columnIndex] != none)
			{
				fields[i][columnIndex] = none;
				--occupiedFields;
				break;
			}
		}
	}
}

// Bewertung der aktuellen Spielsituation mit Hilfe einer Heuristik
int Board::checkWinningSituation(int rowIndex, int columnIndex, Board::FieldState fieldState)
{
	// Indizes für das iterieren durch die 4 möglichen Reihen (zwei Diagonalen, Horizontale und Vertikale)
	int i = rowIndex - 3;
	int k = columnIndex - 3;
	int l = columnIndex + 3;

	// Bonuspotenz für die Bewertung
	int line1Bonus = bonusPower, line2Bonus = bonusPower, line3Bonus = bonusPower, line4Bonus = bonusPower;
	// Summe der Bewertung pro Reihe
	int line1 = 0, line2 = 0, line3 = 0, line4 = 0, maxLine = 0;
	// Anzahl der freien Felder pro Reihe
	int line1Count = 0, line2Count = 0, line3Count = 0, line4Count = 0;
	// Kennzeichen ob Prüfung einer Reihe beendet werden kann
	bool line1End = false, line2End = false, line3End = false, line4End = false;

	// Für jede Reihe durch die drei linken und die drei rechten Nachbarn und das aktuelle Feld selbst iterieren
	for(int m = 0; m < 7 && (!line1End || !line2End || !line3End || !line4End); ++m)
	{
		// Horizontale prüfen
		if(!line1End && k >= 0 && k < boardSize.width && !line1End)
		{
			// Eigener Spielstein gefunden
			if(fields[rowIndex][k] == fieldState)
			{
				line1 += line1Bonus;
				line1Bonus *= bonusPower;
				++line1Count;
			}
			// freies Feld gefunden
			else if(fields[rowIndex][k] == none)
			{
				line1 += bonusPower;
				line1Bonus = bonusPower;
				++line1Count;
			}
			// wenn noch nicht beim aktuellen Feld vorbei
			else if(k < columnIndex)
			{
				line1 = 0;
				line1Bonus = bonusPower;
				line1Count = 0;
			}
			// Prüfung kann beendet werden
			else
				line1End = true;
		}

		// Vertikale
		if(!line2End && i >= 0 && i < boardSize.height)
		{
			// Eigener Spielstein gefunden
			if(fields[i][columnIndex] == fieldState)
			{
				line2 += line2Bonus;
				line2Bonus *= bonusPower;
				++line2Count;
			}
			// freies Feld gefunden
			else if(fields[i][columnIndex] == none)
			{
				line2 += bonusPower;
				line2Bonus = bonusPower;
				++line2Count;
			}
			// wenn noch nicht beim aktuellen Feld vorbei
			else if(i < rowIndex)
			{
				line2 = 0;
				line2Bonus = bonusPower;
				line2Count = 0;
			}
			// Prüfung kann beendet werden
			else
				line2End = true;
		}

		// Diagonale von links unten nach rechts oben
		if(!line3End && i >= 0 && i < boardSize.height && k >= 0 && k < boardSize.width)
		{
			// Eigener Spielstein gefunden
			if(fields[i][k] == fieldState)
			{
				line3 += line3Bonus;
				line3Bonus *= bonusPower;
				++line3Count;
			}
			// freies Feld gefunden
			else if(fields[i][k] == none)
			{
				line3 += bonusPower;
				line3Bonus = bonusPower;
				++line3Count;
			}
			// wenn noch nicht beim aktuellen Feld vorbei
			else if(k < columnIndex)
			{
				line3 = 0;
				line3Bonus = bonusPower;
				line3Count = 0;
			}
			// Prüfung kann beendet werden
			else
				line3End = true;
		}

		// Diagonale von rechts unten nach links oben
		if(!line4End && i >= 0 && i < boardSize.height && l >= 0 && l < boardSize.width)
		{
			// Eigener Spielstein gefunden
			if(fields[i][l] == fieldState)
			{
				line4 += line4Bonus;
				line4Bonus *= bonusPower;
				++line4Count;
			}
			// freies Feld gefunden
			else if(fields[i][l] == none)
			{
				line4 += bonusPower;
				line4Bonus = bonusPower;
				++line4Count;
			}
			// wenn noch nicht beim aktuellen Feld vorbei
			else if(i < rowIndex)
			{
				line4 = 0;
				line4Bonus = bonusPower;
				line4Count = 0;
			}
			// Prüfung kann beendet werden
			else
				line4End = true;
		}

		// Indizes weiterzählen
		++i;
		++k;
		--l;
	}

	// Sollte eine Reihe keine Chance auf vier Steine haben, wird diese mit 0 bewertet
	if(line1Count < 4)
		line1 = 0;
	if(line2Count < 4)
		line2 = 0;
	if(line3Count < 4)
		line3 = 0;
	if(line4Count < 4)
		line4 = 0;

	// höchste Bewertung finden
	maxLine = line1;

	if(line2 > maxLine)
		maxLine = line2;
	if(line3 > maxLine)
		maxLine = line3;
	if(line4 > maxLine)
		maxLine = line4;

	// Spielbewertung zurückgeben
	return maxLine;
}

// aktuellen Spielstatus aktualisieren
void Board::updateGameState()
{
	if(checkWinningSituation(lastRowIndex, lastColumnIndex, lastFieldState) >= 81)
	{
		if(lastFieldState == red)
			gameState = redWon;
		else
			gameState = whiteWon;
	}
	else if(occupiedFields == fieldCount)
		gameState = tie;
}