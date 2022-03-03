void move_gen() {
    moves = {}; 
    for(int i = 0; i < width*height; i++) {
        if(board[i] > 2) {
            int i_temp = i; 
            int times = 0; 
            while(true) { // down
                int y = floor(i_temp / width);
                int x = i_temp % width;
                if(times == 0) {
                    if(board[i_temp + width] > 2) {
                        break; 
                    }
                }
                if(y + 1 == height - 1) {
                    Move move; move.src = i; move.dst = i_temp; moves.push_back(move); break; 
                }
                if(board[i_temp + width] > 2) {
                    Move move; move.src = i; move.dst = i_temp; break; 
                }
                times++; 
            }
            times = 0; 
            i_temp = i; 
        }
    }
    first_move = 0; 
}