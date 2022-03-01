#include <vector>
#include "move.h"
#define Piece char
// 0 = blank, 1 = valley, 2 = white soldier, 3 = black soldier, 4 = white king, 5 = black king, 6 = tower
class Game {
    public: 
    std::vector<Piece> board; 
    std::vector<Move> moves; 
    int width; int height; 
    Game(int width_p, int height_p, std::vector<Piece> board_p) {
        width = width_p; height = height_p; 
        board = board_p; 
    }
    void move(Move move) {
        board[move.dst] = board[move.src]; 
        board[move.src] = 0; 
    } 
};