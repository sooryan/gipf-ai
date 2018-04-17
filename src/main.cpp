#include "gipf.h"

int main() {
	GipfState state = GipfState();

	Minimax<GipfState, GipfMove> a(0.1, 30);
	Minimax<GipfState, GipfMove> b(0.1);

	// state, player a, player b, no of games, verbose, generate gif
	Tester<GipfState, GipfMove> tester(&state, a, b, 1, false, true);
	tester.start();

	return 0;
}
