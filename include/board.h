#pragma once

#include <cmath>
#include <iostream>
#include <map>
#include <string>
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

	Board() {
		pieces_count[player::WHITE] = 12;
		pieces_count[player::BLACK] = 12;
		current_turn = player::WHITE;
	}

	player GetWinner();
	void NextTurn();
	player GetNextTurn();

	/**
	 * Can we move from (letter, number) in direction.
	 */
	bool CanMove(BoardElement elt, direction dir);
	bool Move(BoardElement elt, direction dir);
	BoardElement NextElement(BoardElement elt, direction dir);

	void Resolve();
	void ResolveRow(CapturedRow row);

	void PrintBoard();
};
