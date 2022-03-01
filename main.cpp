#include <iostream>
#include "headers/game.h"
int main() {
	// 0 = blank, 1 = valley, 2 = white soldier, 3 = black soldier, 4 = white king, 5 = black king, 6 = tower
	std::vector<char> board = {
		3, 3, 5, 3, 3, 
		0, 0, 0, 0, 0, 
		0, 0, 1, 0, 0, 
		0, 0, 0, 0, 0, 
		3, 3, 4, 3, 3
	}; 
	Game game(5, 5, board); 
	return 0; 
}