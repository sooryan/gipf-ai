#include "gipf.h"

int main() {
	GipfState state = GipfState("_____"
	                            "______"
	                            "_______"
	                            "________"
	                            "_________"
	                            "________"
	                            "_______"
	                            "______"
	                            "_____");

	Minimax<GipfState, GipfMove> a(0.1, 30);
	Minimax<GipfState, GipfMove> b(0.1, 30);

	Tester<GipfState, GipfMove> tester(&state, a, b);
	tester.start();

	return 0;
}
