#include "board.h"

std::map<BoardElement, std::vector<direction>> Board::possible_directions = {
    {{0, 0}, {direction::NE}},
    {{1, 0}, {direction::N, direction::NE}},
    {{2, 0}, {direction::N, direction::NE}},
    {{3, 0}, {direction::N, direction::NE}},
    {{4, 0}, {direction::N}},
    {{5, 0}, {direction::NW, direction::N}},
    {{6, 0}, {direction::NW, direction::N}},
    {{7, 0}, {direction::NW, direction::N}},
    {{8, 0}, {direction::NW}},
    {{8, 1}, {direction::SW, direction::NW}},
    {{8, 2}, {direction::SW, direction::NW}},
    {{8, 3}, {direction::SW, direction::NW}},
    {{8, 4}, {direction::SW}},
    {{7, 5}, {direction::S, direction::SW}},
    {{6, 6}, {direction::S, direction::SW}},
    {{5, 7}, {direction::S, direction::SW}},
    {{4, 8}, {direction::S}},
    {{3, 7}, {direction::SE, direction::S}},
    {{2, 6}, {direction::SE, direction::S}},
    {{1, 5}, {direction::SE, direction::S}},
    {{0, 4}, {direction::SE}},
    {{0, 3}, {direction::NE, direction::SE}},
    {{0, 2}, {direction::NE, direction::SE}},
    {{0, 1}, {direction::NE, direction::SE}},
};

std::vector<Row> Board::rows = {
    {4, 0, direction::NE}, {3, 0, direction::NE}, {2, 0, direction::NE},
    {1, 0, direction::NE}, {0, 0, direction::NE}, {0, 1, direction::NE},
    {0, 2, direction::NE}, {0, 3, direction::NE}, {0, 4, direction::NE},
    {0, 0, direction::SE}, {0, 1, direction::SE}, {0, 2, direction::SE},
    {0, 3, direction::SE}, {0, 4, direction::SE}, {1, 5, direction::SE},
    {2, 6, direction::SE}, {3, 7, direction::SE}, {4, 8, direction::SE},
    {0, 0, direction::N},  {1, 0, direction::N},  {2, 0, direction::N},
    {3, 0, direction::N},  {4, 0, direction::N},  {5, 0, direction::N},
    {6, 0, direction::N},  {7, 0, direction::N},  {8, 0, direction::N}};

/**
 *     [0,0,0,0,0]
 *    [0,2,0,0,1,0]
 *   [0,0,0,0,0,0,0]
 *  [0,0,0,0,0,0,0,0]
 * [0,1,0,0,0,0,0,2,0]
 *  [0,0,0,0,0,0,0,0]
 *   [0,0,0,0,0,0,0]
 *    [0,2,0,0,1,0]
 *     [0,0,0,0,0]
 */
std::vector<std::vector<player>> Board::pieces = {
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE},
    {player::NONE, player::BLACK, player::NONE, player::NONE, player::WHITE,
     player::NONE},
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE,
     player::NONE, player::NONE},
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE,
     player::NONE, player::NONE, player::NONE},
    {player::NONE, player::WHITE, player::NONE, player::NONE, player::NONE,
     player::NONE, player::NONE, player::BLACK, player::NONE},
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE,
     player::NONE, player::NONE, player::NONE},
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE,
     player::NONE, player::NONE},
    {player::NONE, player::BLACK, player::NONE, player::NONE, player::WHITE,
     player::NONE},
    {player::NONE, player::NONE, player::NONE, player::NONE, player::NONE}};

bool Board::InBoard(BoardElement elt) {
	if (elt.letter < 1 or elt.letter > 7) {
		return false;
	}
	if (elt.number < 1 or elt.number > (7 - abs(elt.letter - 4))) {
		return false;
	}
	return true;
}

player Board::GetWinner() {
	if (pieces_count[player::WHITE] <= 0)
		return player::BLACK;
	if (pieces_count[player::BLACK] <= 0)
		return player::WHITE;
	return player::NONE;
}

player Board::NextTurn() {
	if (current_turn == player::WHITE)
		return player::BLACK;
	return player::WHITE;
}

BoardElement Board::NextElement(BoardElement elt, direction dir) {
	switch (dir) {
	case direction::N:
		return BoardElement(elt.letter, elt.number + 1);
	case direction::NE:
		if (elt.letter < 4) {
			return BoardElement(elt.letter + 1, elt.number + 1);
		}
		return BoardElement(elt.letter + 1, elt.number);
	case direction::SE:
		if (elt.letter < 4) {
			return BoardElement(elt.letter + 1, elt.number);
		}
		return BoardElement(elt.letter + 1, elt.number - 1);
	case direction::S:
		return BoardElement(elt.letter, elt.number - 1);
	case direction::SW:
		if (elt.letter < 5) {
			return BoardElement(elt.letter - 1, elt.number - 1);
		}
		return BoardElement(elt.letter - 1, elt.number);
	case direction::NW:
		if (elt.letter < 5) {
			return BoardElement(elt.letter - 1, elt.number);
		}
		return BoardElement(elt.letter - 1, elt.number + 1);
	}
}

bool Board::CanMove(BoardElement elt, direction dir) {
	BoardElement next = NextElement(elt, dir);
	while (InBoard(next)) {
		int n_i = next.letter;
		int n_j = next.number;
		player cur_color = pieces[n_i][n_j];
		if (cur_color == player::NONE)
			return true;
		next = NextElement(elt, dir);
	}
	return false;
}

bool Board::Move(BoardElement elt, direction dir, player ply) {
	if (!CanMove(elt, dir)) {
		return false;
	}
	pieces[elt.letter][elt.number] = ply;
	SlidePieces(elt, dir);
	if (ply == player::WHITE) {
		pieces_count[player::WHITE]--;
	} else if (ply == player::BLACK) {
		pieces_count[player::BLACK]--;
	}
	return true;
}

void Board::SlidePieces(BoardElement elt, direction dir) {
	int i = elt.letter;
	int j = elt.number;
	player last_color = pieces[i][j];
	pieces[i][j] = player::NONE;
	BoardElement next = NextElement(elt, dir);
	while (InBoard(next)) {
		int n_i = next.letter;
		int n_j = next.number;
		player cur_color = pieces[n_i][n_j];
		pieces[n_i][n_j] = last_color;
		if (cur_color == player::NONE)
			break;
		last_color = cur_color;
		next = NextElement(elt, dir);
	}
}

player Board::FourInARow(Row row) {
	int count = 0;
	player last_color = pieces[row.elt.letter][row.elt.number];
	BoardElement next = NextElement(row.elt, row.dir);
	while (InBoard(next)) {
		player color = pieces[next.letter][next.number];
		if (color == last_color) {
			count--;
			if (count == 0) {
				return color;
			}
		} else {
			count = 4;
			last_color = color;
		}
		next = NextElement(next, row.dir);
	}
	return player::NONE;
}

void Board::ResolveRow(CapturedRow row) {
	auto elt = row.row.elt;
	auto dir = row.row.dir;
	auto color = row.color;

	BoardElement next = NextElement(elt, dir);
	while (InBoard(next)) {
		int n_i = next.letter;
		int n_j = next.number;
		if (pieces[n_i][n_j] == color) {
			pieces_count[color]++;
		}
		pieces[n_i][n_j] = player::NONE;
		next = NextElement(elt, dir);
	}
}

void Board::Resolve() {
	std::vector<CapturedRow> crow;
	for (auto row : rows) {
		auto color = FourInARow(row);
		switch (color) {
		case player::WHITE:
		case player::BLACK:
			crow.push_back({row, color});
			break;
		case player::NONE:
			continue;
		}
	}
	if (crow.size() == 1) {
		ResolveRow(crow[0]);
	}
	// TODO: It may occur that two rows are lined up at the same time.
	// If these rows don't intersect, both must be captured; if they do
	// intersect, the player playing with that color may choose which
	// row he will take.

}
