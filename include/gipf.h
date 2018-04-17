#include <boost/functional/hash.hpp>
#include <climits>
#include <cmath>
#include <string>

#include "gtsa.hpp"
#include "utils.h"

struct GipfMove : public Move<GipfMove> {
	int64_t elt;
	direction dir;

	GipfMove() {}

	GipfMove(int64_t elt, direction dir) : elt(elt), dir(dir) {}

	void read(istream &stream = cin) override {
		if (&stream == &cin) {
			cout << "Enter row, column and direction of your move (A1 NE): ";
		}
		int row, column;
		stream >> row >> column >> dir;
		elt = xytoint(row, column);
	}

	ostream &to_stream(ostream &os) const override {
		return os << elt << ", " << m[dir];
	}

	bool operator==(const GipfMove &rhs) const override {
		return elt == rhs.elt && dir == rhs.dir;
	}

	size_t hash() const override {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(elt));
		hash_combine(seed, hash_value(dir));
		return seed;
	}
};

struct Board {
	uint64_t board = 0;

	Board() {}

	Board(const Board &other) { board = other.board; }

	void set(int x, int y, uint64_t value) {
		board = board & (1 << (60 - (COLSUMS[x] + y)));
	}

	bool get(int x, int y) const {
		uint64_t i = 60 - (COLSUMS[x] + y);
		return (board >> i) & 1;
	}

	bool operator==(const Board &other) const { return board == other.board; }

	bool CanMove(int64_t elt, direction dir) const {
		int64_t next = next_element[dir][elt];
		while (InBoard(next)) {
			if ((board & next) == 0)
				return true;
			next = next_element[dir][next];
		}
		return false;
	}

	void SlidePieces(int64_t elt, direction dir) {
		int64_t curr = elt;
		int64_t next = next_element[dir][elt];
		while (InBoard(next)) {
			bool next_empty = (board & next) == 0;

			board = (board | next) & (~curr);
			if (next_empty)
				break;
			curr = next;
			next = next_element[dir][next];
		}
	}

	bool InBoard(int64_t x) const { return (x & 2271516307835194431) == 0; }
};

size_t hash_value(const Board &board) {
	hash<uint64_t> hash_fn;
	return hash_fn(board.board);
}

struct GipfState : public State<GipfState, GipfMove> {

	Board board_1, board_2;
	int64_t pieces_left_1, pieces_left_2;
	GipfState() : State(PLAYER_1) {
		pieces_left_1 = 15;
		pieces_left_2 = 15;
	}

	GipfState(const string &init_string) : State(PLAYER_1) {
		pieces_left_1 = 15;
		pieces_left_2 = 15;

		const unsigned long length = init_string.length();
		const unsigned long correct_length = 61;
		if (length != correct_length) {
			throw invalid_argument("Initialization string length must be " +
			                       to_string(correct_length));
		}
		for (auto c : init_string) {
			if (c != PLAYER_1 && c != PLAYER_2 && c != EMPTY) {
				throw invalid_argument(string("Undefined symbol used: '") + c +
				                       "'");
			}
		}

		for (int y = 0; y < 9; ++y) {
			for (int x = 0; x < 9 - abs(y - 4); ++x) {
				const char c = init_string[y * COLLEN[y] + x];
				if (c == PLAYER_1) {
					board_1.set(x, y, 1);
					pieces_left_1--;
				} else if (c == PLAYER_2) {
					board_2.set(x, y, 1);
					pieces_left_2--;
				}
			}
		}
	}

	GipfState clone() const override {
		GipfState clone = GipfState();
		clone.board_1 = Board(board_1);
		clone.board_2 = Board(board_2);
		clone.pieces_left_1 = pieces_left_1;
		clone.pieces_left_2 = pieces_left_2;
		clone.player_to_move = player_to_move;
		return clone;
	}

	int get_reserve_value(int pieces_left) const {
		return pieces_left * (280 - std::floor(std::cbrt(pieces_left - 1) * 20));
	}

	int get_goodness() const override {
		if (is_terminal()) {
			if (is_winner(player_to_move)) {
				return INT_MAX;
			} else if (is_winner(get_enemy(player_to_move))) {
				return INT_MIN;
			} else {
				return 0;
			}
		}

		int score = get_reserve_value(pieces_left_1) - get_reserve_value(pieces_left_2);

		auto pieces_board_1 = no_of_set_bits(board_1.board);
		auto pieces_board_2 = no_of_set_bits(board_2.board);
		score += (pieces_board_1 - pieces_board_2) * 230;

		int pieces_dead_1 = 15 - pieces_left_1 - pieces_board_1;
		int pieces_dead_2 = 15 - pieces_left_2 - pieces_board_2;
		score += (pieces_dead_2 - pieces_dead_1) * (pieces_dead_2 + pieces_dead_1) * 10;

		if (player_to_move == PLAYER_2) {
			score *= -1;
		}

		return score;
	}

	vector<GipfMove> get_legal_moves(int max_moves = INF) const override {
		auto &board = player_to_move == PLAYER_1 ? board_1 : board_2;
		int available_moves = 42;
		if (max_moves > available_moves) {
			max_moves = available_moves;
		}
		vector<GipfMove> moves;
		for (auto eltdir : possible_directions) {
			for (auto dir : eltdir.second) {
				if (board.CanMove(eltdir.first, dir)) {
					moves.push_back(GipfMove(eltdir.first, dir));
				}
			}
		}
		return moves;
	}

	char get_enemy(char player) const override {
		return (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
	}

	bool is_terminal() const override {
		return is_winner(player_to_move) ||
		       is_winner(get_enemy(player_to_move));
	}

	bool is_winner(char player) const override {
		int64_t enemy_pieces_left =
		    (player == PLAYER_1) ? pieces_left_2 : pieces_left_1;
		if (enemy_pieces_left <= 0)
			return true;
		return false;
	}

	void make_move(const GipfMove &move) override {
		auto &board = (player_to_move == PLAYER_1) ? board_1 : board_2;
		auto &pieces_left =
		    (player_to_move == PLAYER_1) ? pieces_left_1 : pieces_left_2;
		board.board |= move.elt;
		board.SlidePieces(move.elt, move.dir);
		pieces_left--;
		ResolveBoard();
		player_to_move = get_enemy(player_to_move);
	}

	void ResolveBoard() {
		auto &pieces_left =
		    (player_to_move == PLAYER_1) ? pieces_left_1 : pieces_left_2;
		for (auto row : four_in_a_row_cases) {
			if (((~board_1.board) & row.first) == 0) {
				int64_t count = 0;
				if (player_to_move == PLAYER_1) {
					count = board_1.board & row.second;
				} else {
					count = board_2.board & row.second;
				}
				board_1.board &= row.second;
				board_2.board &= row.second;
				pieces_left += no_of_set_bits(count);
				break;
			}
		}
	}

	void undo_move(const GipfMove &move) override {
		make_move(GipfMove(move.elt, reverse_direction[move.dir]));
	}

	bool is_empty(int x, int y) const {
		return board_1.get(x, y) == 0 && board_2.get(x, y) == 0;
	}

	ostream &to_stream(ostream &os) const override {

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9 - abs(i - 4); j++) {
				int i_eff = 19 - abs(i - 4) - (j * 2);
				int j_eff = i * 4 + 4;
				if (board_1.board & xytoint(i, j)) {
					board_string[i_eff][j_eff] = 'W';
				} else if (board_2.board & xytoint(i, j)) {
					board_string[i_eff][j_eff] = 'B';
				} else {
					board_string[i_eff][j_eff] = '*';
				}
			}
		}
		os << " +-------------------------------------+ \n";
		os << "         Pieces Left: B(" << pieces_left_2 << ") W("
		   << pieces_left_1 << ")\n";
		for (auto row : board_string) {
			os << row << std::endl;
		}

		os << player_to_move << endl;
		return os;
	}

	bool operator==(const GipfState &other) const override {
		return board_1 == other.board_1 && board_2 == other.board_2 &&
		       player_to_move == other.player_to_move;
	}

	size_t hash() const {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(board_1));
		hash_combine(seed, hash_value(board_2));
		hash_combine(seed, hash_value(pieces_left_1));
		hash_combine(seed, hash_value(pieces_left_2));
		hash_combine(seed, hash_value(player_to_move));
		return seed;
	}
};
