#include "board.h"
#include "utils.h"

using namespace std;
#define clear() cout << "\033[H\033[J";

int main() {
	Board board;
	char l;
	int n;
	string dir;
	clear();
	std::string buffer(22, ' ');
	do {
		board.PrintBoard();
		cout << buffer << "  ["
		     << (board.GetNextTurn() == player::WHITE ? "#" : "&")
		     << "] Enter your move eg(A1 NE): ";
		cin >> l >> n >> dir;
		getchar();
		board.Move(BoardElement(l - 'A', n - 1), m[dir]);
		clear();
	} while (true);
}
