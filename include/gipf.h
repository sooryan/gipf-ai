#include <bitset>
#include <boost/functional/hash.hpp>
#include <climits>
#include <cmath>
#include <string>

#include "gtsa.hpp"
#include "utils.h"

using llint = long long int;
using ullint = unsigned long long int;

struct GipfMove : public Move<GipfMove> {
	llint elt;
	direction dir;
	vector<llint> captures;

	GipfMove() {}

	GipfMove(llint elt, direction dir, vector<llint> captures) : elt(elt), dir(dir), captures(captures) {}

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
		return elt == rhs.elt && dir == rhs.dir && captures == rhs.captures;
	}

	size_t hash() const override {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(elt));
		hash_combine(seed, hash_value(dir));
		for (auto capture : captures) {
			hash_combine(seed, hash_value(capture));
		}
		return seed;
	}
};

struct Board {
	ullint board = 0;

	Board() {}

	Board(const Board &other) { board = other.board; }

	void set(int x, int y, ullint value) {
		board = board & (1 << (60 - (COLSUMS[x] + y)));
	}

	bool get(int x, int y) const {
		ullint i = 60 - (COLSUMS[x] + y);
		return (board >> i) & 1;
	}

	bool operator==(const Board &other) const { return board == other.board; }

	bool CanMove(llint elt, direction dir) const {
		llint next = next_element[dir][elt];
		while (in_board(next) && next != 0) {
			if ((board & next) == 0)
				return true;
			next = next_element[dir][next];
		}
		return false;
	}

	void SlidePieces(llint elt, direction dir, Board &combined) {
		llint next = next_element[dir][elt];
		llint mask_board = next;
		llint mask_combined = next;
		llint curr = elt;
		while (in_board(next) && (next != 0)) {
			bool next_empty = (combined.board & next) == 0;
			if (board & curr) {
				board &= (~curr);
				mask_board |= next;
			}
			mask_combined |= next;
			if (next_empty) {
				break;
			}
			curr = next;
			next = next_element[dir][next];
		}
		board |= mask_board;
		combined.board |= mask_combined;
	}
};

size_t hash_value(const Board &board) {
	hash<ullint> hash_fn;
	return hash_fn(board.board);
}

struct GipfState : public State<GipfState, GipfMove> {

	Board board_1, board_2, combined;
	llint pieces_left_1, pieces_left_2;
	static std::vector<GipfState> history;
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
		combined.board = board_1.board | board_2.board;
	}

	GipfState clone() const override {
		return *this;
	}

	int get_reserve_value(int pieces_left) const {
		return pieces_left *
		       (280 - std::floor(std::cbrt(pieces_left - 1) * 20));
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

		int score =
		    get_reserve_value(pieces_left_1) - get_reserve_value(pieces_left_2);

		auto pieces_board_1 = no_of_set_bits(board_1.board);
		auto pieces_board_2 = no_of_set_bits(board_2.board);
		score += (pieces_board_1 - pieces_board_2) * 230;

		int pieces_dead_1 = 15 - pieces_left_1 - pieces_board_1;
		int pieces_dead_2 = 15 - pieces_left_2 - pieces_board_2;
		score += (pieces_dead_2 - pieces_dead_1) *
		         (pieces_dead_2 + pieces_dead_1) * 10;

		llint x = 1LL<<60;
		while (x > 0) {
			score += position_scores[board_1.board & x];
			score -= position_scores[board_2.board & x];
			x >>= 1;
		}

		if (player_to_move == PLAYER_2) {
			score *= -1;
		}

		return score;
	}

	vector<GipfMove> get_legal_moves(int max_moves = INF) const override {
		vector<GipfMove> moves;
		for (auto eltdir : possible_directions) {
			for (auto dir : eltdir.second) {
				if (!combined.CanMove(eltdir.first, dir))
					continue;

				auto dirty_state = *this;
				dirty_state.SlidePieces(eltdir.first, dir);
				auto capture_mask_sets = dirty_state.GetCaptureMaskSets();

				if (capture_mask_sets.empty()) {
					moves.emplace_back(eltdir.first, dir, vector<llint>(0));
				}
				else {
					for (const auto& mask_set : capture_mask_sets) {
						moves.emplace_back(eltdir.first, dir, mask_set);
					}
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
		llint enemy_pieces_left =
		    (player == PLAYER_1) ? pieces_left_2 : pieces_left_1;
		if (enemy_pieces_left <= 0)
			return true;
		return false;
	}

	void SlidePieces(llint elt, direction dir) {
		assert(combined.CanMove(elt, dir));

		auto &board = (player_to_move == PLAYER_1) ? board_1 : board_2;
		auto &other_board = (player_to_move == PLAYER_1) ? board_2 : board_1;
		auto &pieces_left =
		    (player_to_move == PLAYER_1) ? pieces_left_1 : pieces_left_2;

		auto pieces_board_1 = no_of_set_bits(board.board);
		auto pieces_board_2 = no_of_set_bits(other_board.board);

		board.SlidePieces(elt, dir, combined);
		other_board.board = combined.board & (~board.board);

		assert((board_1.board & board_2.board) == 0);
		assert(pieces_board_1 == (no_of_set_bits(board.board) - 1));
		assert(pieces_board_2 == (no_of_set_bits(other_board.board)));

		pieces_left--;
	}

	void make_move(const GipfMove &move) override {
		history.push_back(*this);
		SlidePieces(move.elt, move.dir);

		for (auto capture : move.captures) {
			ResolveRow(capture);
		}

		player_to_move = get_enemy(player_to_move);
	}

	void ResolveRow(llint row) {
		llint count1 = no_of_set_bits(board_1.board & row);
		llint count2 = no_of_set_bits(board_2.board & row);
		if (count2 >= 4) {
		  pieces_left_2 += count2;
		} else {
		  pieces_left_1 += count1;
		}
		board_1.board &= ~row;
		board_2.board &= ~row;
		combined.board = board_1.board | board_2.board;
	}

	vector<vector<llint>> GetCaptureMaskSets() {
		auto &board1 = (player_to_move == PLAYER_1) ? board_1 : board_2;
		auto &board2 = (player_to_move == PLAYER_1) ? board_2 : board_1;
		vector<std::reference_wrapper<Board>> boards = {board1, board2};
		vector<llint> available_four_in_a_rows;
		vector<vector<llint>> capture_mask_sets;

		for (auto board : boards) {
			for (auto row : four_in_a_row_cases) {
				if (((~board.get().board) & row.first) == 0) {
					llint capture_mask = 0;
					auto direlt = row.second;
					auto next = next_element[direlt.first][direlt.second];
					bool matched = false;
					while (combined.board & next or (!matched)) {
						if (combined.board & next) {
							capture_mask |= next;
							matched = true;
						}
						next = next_element[direlt.first][next];
					}
					available_four_in_a_rows.push_back(capture_mask);
				}
			}
		}

		for (auto row : available_four_in_a_rows) {
			bool found_set = false;

			for (auto& mask_set : capture_mask_sets) {
				bool does_intersect = false;

				for (auto mask : mask_set) {
					if ((row & mask) != 0) {
						does_intersect = true;
						break;
					}
				}

				if (!does_intersect) {
					mask_set.push_back(row);
					found_set = true;
				}
			}

			if (!found_set) {
				capture_mask_sets.push_back(vector<llint>(1, row));
			}
		}

		return capture_mask_sets;
	}

	void undo_move(const GipfMove &move) override {
		*this = history.back();
		history.pop_back();
		return;
	}

	bool is_empty(int x, int y) const {
		return board_1.get(x, y) == 0 && board_2.get(x, y) == 0;
	}

	ostream &to_stream(ostream &os) const override {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9 - abs(i - 4); j++) {
				int i_eff = 19 - abs(i - 4) - (j * 2);
				int j_eff = i * 4 + 4;
				if (board_1.get(i, j)) {
					board_string[i_eff][j_eff] = 'W';
				} else if (board_2.get(i, j)) {
					board_string[i_eff][j_eff] = 'B';
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

	size_t hash() const override {
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

std::vector<GipfState> GipfState::history = std::vector<GipfState>(0);
