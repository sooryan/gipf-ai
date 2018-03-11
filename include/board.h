#pragma once

#include <cmath>
#include <map>
#include <vector>

enum class player {
	WHITE = 0,
	BLACK = 1,
	NONE,
};

/**
       N
  SW   |   NE
     >-o-<
  NW   |   SE
       S
*/
enum class direction {
	N,
	S,
	NE,
	NW,
	SE,
	SW,
};

struct BoardElement {
	int letter;
	int number;
	BoardElement(int letter, int number) : letter(letter), number(number) {}
	bool operator<(const BoardElement &other) const {
		return letter < other.letter;
	}
};

struct Row {
	BoardElement elt;
	direction dir;
	Row(int letter, int number, direction dir)
	    : elt(BoardElement(letter, number)), dir(dir) {}
	Row(BoardElement elt, direction dir) : elt(elt), dir(dir) {}
};

struct CapturedRow {
	Row row;
	player color;
	CapturedRow(Row row, player color) : row(row), color(color){};
};

class Board {
  private:
	std::map<player, int> pieces_count;
	player current_turn;
	/**
	 * A lookup table for the possible starting directions
	 */
	static std::map<BoardElement, std::vector<direction>> possible_directions;
	/**
	 * uniquely identify all rows
	 */
	static std::vector<Row> rows;
	/**
	 * The actual board and the pieces
	 */
	static std::vector<std::vector<player>> pieces;

	/**
	 * Returns true if the given (letter, number) pair is in the board
	 */
	bool InBoard(BoardElement elt);
	player FourInARow(Row row);

	/**
	 * Slide the pieces one spot in direction starting at (letter, number)
	 */
	void SlidePieces(BoardElement elt, direction dir);

  public:
	Board() {
		pieces_count[player::WHITE] = 15;
		pieces_count[player::BLACK] = 15;
		current_turn = player::WHITE;
		// pieces.push_back(std::vector<player>(5, player::NONE));
		// pieces.push_back(std::vector<player>(6, player::NONE));
		// pieces.push_back(std::vector<player>(7, player::NONE));
		// pieces.push_back(std::vector<player>(8, player::NONE));
		// pieces.push_back(std::vector<player>(9, player::NONE));
		// pieces.push_back(std::vector<player>(8, player::NONE));
		// pieces.push_back(std::vector<player>(7, player::NONE));
		// pieces.push_back(std::vector<player>(6, player::NONE));
		// pieces.push_back(std::vector<player>(5, player::NONE));
	}

	player GetWinner();
	player NextTurn();

	/**
	 * Can we move from (letter, number) in direction.
	 */
	bool CanMove(BoardElement elt, direction dir);
	bool Move(BoardElement elt, direction dir, player ply);
	BoardElement NextElement(BoardElement elt, direction dir);

	void Resolve();
	void ResolveRow(CapturedRow row);
};
