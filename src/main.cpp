#include "gipf.h"

int main() {
	GipfState state = GipfState();

	MonteCarloTreeSearch<GipfState, GipfMove> a(0.2, 38);
	MonteCarloTreeSearch<GipfState, GipfMove> b(0.1, 55);

	// state, player a, player b, no of games, verbose, generate gif
	Tester<GipfState, GipfMove> tester(&state, a, b, 3, false, true);
	tester.start();

	return 0;
}
