#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "headers/game.h"
int main() {
	// 0 = blank, 1, = wall, 2 = valley, 3 = white stone, 4 = black stone, 5 = white king, 6 = black king, 6 = wall
	// std::vector<char> board = {
	// 	1, 0, 0, 0, 0, 
	// 	0, 0, 0, 0, 0, 
	// 	0, 0, 0, 0, 0, 
	// 	0, 0, 0, 0, 0, 
	// 	0, 0, 0, 0, 0
	// }; 
	// Game game(5, 5, board); 
	// game.move_gen(); 
	// if(game.moves.size() > 0) {
	// 	std::cout << game.moves[0].src << ", " << game.moves[1].dst << std::endl; 
	// }
	float window_size[2] = {700, 700}; 
	int num_x = 7; int num_y = 7; 
	int colors[2][3] = {{180, 135, 98}, {240, 217, 181}}; 
	bool smaller = 0; 
	sf::RenderWindow window(sf::VideoMode(window_size[0], window_size[1]), "Kings Valley");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				window.close();
			}
			if(event.type == sf::Event::Resized) { 
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window_size[0] = event.size.width; 
				window_size[1] = event.size.height; 
				if(window_size[smaller] > window_size[1 - smaller]) {
					smaller = 1 - smaller; 
				}
        		window.setView(sf::View(visibleArea));
			}
		}
		window.clear(); 
		float square_size = window_size[smaller]/num_x; 
		int even = 0; 
		float pos_x = 0; float pos_y = 0;
		for(int i = 0; i <= num_y - 1; i++) {
			for(int j = 0; j <= num_x - 1; j++) {
				sf::RectangleShape square(sf::Vector2f(square_size, square_size));
				square.setPosition(pos_x, pos_y); 
				square.setFillColor(sf::Color(colors[even][0], colors[even][1], colors[even][2]));
				pos_x += square_size; 
				even = 1 - even; 
				window.draw(square); 
			}
			pos_x = 0; 
			pos_y += square_size; 
			if((int)num_x % 2 == 0) {
				even = 1 - even; 
			}
		}
		window.display();
	}
	return 0; 
}