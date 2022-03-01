class Move {
    public:
    int src; int dst; 
    Move(char x1, char y1, char x2, char y2, char width) {
        src = x1 + y1*width; 
        dst = x2 + y2*width; 
    }
};