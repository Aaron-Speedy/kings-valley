#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "headers/game.h"
int main() {
	// 0 = blank, 1, = wall, 2 = valley, 3 = white stone, 4 = black stone, 5 = white king, 6 = black king
	// std::vector<char> board = {
		// 4, 4, 4, 4, 5, 4, 4, 4, 4,
		// 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// 3, 0, 1, 0, 0, 0, 1, 0, 0,
		// 0, 0, 0, 0, 2, 0, 0, 0, 0,
		// 0, 0, 1, 0, 0, 0, 1, 0, 4,
		// 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// 3, 3, 3, 3, 6, 3, 3, 3, 3
	// };
	// int num[2] = {9, 9};
	std::vector<char> board = {
		4, 4, 4, 5, 4, 4, 4, 
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 1, 0,
		0, 0, 0, 2, 0, 0, 0,
		0, 1, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0,
		3, 3, 3, 6, 3, 3, 3, 

	}; 
	int num[2] = {7, 7}; 
	Game game(num[0], num[1], board); 
	// game.move_gen();  
	// if(game.moves.size() > 0) {
	// 	std::cout << game.moves[0].src << ", " << game.moves[1].dst << std::endl; 
	// }
	float window_size[2] = {700, 700}; 
	bool bigger = 0; 
	if(num[0] < num[1]) {
		bigger = 1; 
	}
	int colors[2][3] = {{180, 135, 98}, {240, 217, 181}}; 
	
	int stone_colors[2][3] = {{160, 115, 78}, {240, 217, 181}}; 
	int border_stone_colors[2][3] = {{90, 67, 49}, {120, 108, 90}}; 

	int king_colors[2][3] = {{160, 115, 78}, {240, 217, 181}}; 
	int border_king_colors[2][3] = {{90, 67, 49}, {120, 108, 90}}; 

	int wall_colors[3] = {90, 67, 49};
	int border_wall_colors[3] = {240, 217, 181}; 

	int step_colors[2][3] = {{0, 0, 0}, {255, 255, 255}}; 
	int border_step_colors[2][3] = {{0, 0, 0}, {255, 255, 255}}; 

	int valley_colors[3] = {240-20, 217-20, 181-20};

	float stone_size = 1/2.8; 
	float king_size = 1/1.5; 
	float wall_size = 1/1.5;
	float step_size = 1/1.6; 

	float stone_border_size = 1.0f/70.0f; 
	float king_border_size = 1.0f/70.0f; 
	float wall_border_size = 1.0f/70.0f; 
	float step_border_size = 1.0f/70.0f; 

	float select_color[2] = {-1/8.0f, 1/4.0f};


	bool smaller = 0; 
	std::vector<int> mouse_pos_1 = {0, 0}; 
	std::vector<int> mouse_pos_2 = {0, 0}; 
	bool mouse_clicked_1 = false; 
	bool mouse_clicked_2 = false; 
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 15; 
	sf::RenderWindow window(sf::VideoMode(window_size[0], window_size[1]), "Kings Valley", sf::Style::Default, settings);
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
			if(event.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					mouse_clicked_1 = false; 
				}
				else {
					if(mouse_clicked_1) {
						mouse_clicked_2 = true; 
						mouse_pos_2 = {event.mouseButton.x, event.mouseButton.y}; 
					}
					else {
						mouse_clicked_1 = true; 
						mouse_pos_1 = {event.mouseButton.x, event.mouseButton.y}; 
					}
				}
				
			}
		}
		window.clear(); 
		float square_size = window_size[smaller]/num[bigger]; 
		int even = 1; 
		float pos[2] = {0, 0}; 
		for(int i = 0; i <= num[1 - bigger] - 1; i++) {
			for(int j = 0; j <= num[bigger] - 1; j++) {
				sf::RectangleShape square(sf::Vector2f(square_size, square_size));
				square.setPosition(pos[0], pos[1]); 
				square.setFillColor(sf::Color(colors[even][0], colors[even][1], colors[even][2]));
				if(mouse_clicked_1) {
					int x_1 = floor(mouse_pos_1[0] / square_size); 
					int y_1 = floor(mouse_pos_1[1] / square_size); 
					if(i == y_1 && j == x_1) {
						square.setFillColor(sf::Color(colors[even][0] + (255 - colors[even][0])*select_color[even], colors[even][1] + (255 - colors[even][1])*select_color[even], colors[even][2] + (255 - colors[even][2])*select_color[even]));
					}
				}
				window.draw(square); 
				switch(board[j + i*num[0]]) {
					case 3: 
						{
						sf::CircleShape circle(square_size*stone_size); 
						circle.setFillColor(sf::Color(stone_colors[1][0], stone_colors[1][1], stone_colors[1][2]));
						circle.setOutlineThickness(square_size*stone_border_size);
						circle.setOutlineColor(sf::Color(border_stone_colors[1][0], border_stone_colors[1][1], border_stone_colors[1][2]));
						circle.setPosition(pos[0] + square_size/2 - (square_size*stone_size), pos[1] + square_size/2 - (square_size*stone_size));
						circle.setPointCount(600);
						window.draw(circle); 
						break; 
						}
					case 4: 
						{
						sf::CircleShape circle(square_size*stone_size); 
						circle.setFillColor(sf::Color(stone_colors[0][0], stone_colors[0][1], stone_colors[0][2]));
						circle.setOutlineThickness(square_size*stone_border_size);
						circle.setOutlineColor(sf::Color(border_stone_colors[0][0], border_stone_colors[0][1], border_stone_colors[0][2]));
						circle.setPosition(pos[0] + square_size/2 - (square_size*stone_size), pos[1] + square_size/2 - (square_size*stone_size));
						circle.setPointCount(600);
						window.draw(circle); 
						break; 
						}
					case 5: 
						{
						sf::RectangleShape square(sf::Vector2f(square_size*king_size, square_size*king_size));
						square.setFillColor(sf::Color(king_colors[1][0], king_colors[1][1], king_colors[1][2]));
						square.setOutlineThickness(square_size*king_border_size);
						square.setOutlineColor(sf::Color(border_king_colors[1][0], border_king_colors[1][1], border_king_colors[1][2]));
						square.setPosition(pos[0] + square_size/2 - (square_size*king_size)/2, pos[1]  + square_size/2 - (square_size*king_size)/2);
						window.draw(square); 
						break; 
						}
					case 6: 
						{
						sf::RectangleShape square(sf::Vector2f(square_size*king_size, square_size*king_size));
						square.setFillColor(sf::Color(king_colors[0][0], king_colors[0][1], king_colors[0][2]));
						square.setOutlineThickness(square_size*king_border_size);
						square.setOutlineColor(sf::Color(border_king_colors[0][0], border_king_colors[0][1], border_king_colors[0][2]));
						square.setPosition(pos[0] + square_size/2 - (square_size*king_size)/2, pos[1] + square_size/2 - (square_size*king_size)/2);
						window.draw(square); 
						break; 
						}
					case 1: 
						{
						sf::RectangleShape square(sf::Vector2f(square_size*wall_size, square_size*wall_size));
						square.setFillColor(sf::Color(wall_colors[0], wall_colors[1], wall_colors[2]));
						square.setOutlineColor(sf::Color(border_wall_colors[0], border_wall_colors[1], border_wall_colors[2]));
						square.setOutlineThickness(square_size*wall_border_size);
						square.setPosition(pos[0] + square_size/2 - (square_size*wall_size)/2, pos[1] + square_size/2 - (square_size*wall_size)/2);
						window.draw(square); 
						break; 
						}
					case 2: 
						{
						sf::RectangleShape square(sf::Vector2f(square_size, square_size));
						square.setFillColor(sf::Color(valley_colors[0], valley_colors[1], valley_colors[2]));
						square.setPosition(pos[0], pos[1]);
						window.draw(square); 
						break; 
						}
				}
				pos[bigger] += square_size; 
				even = 1 - even; 
			}
			pos[bigger] = 0; 
			pos[1 - bigger] += square_size; 
			if((int)num[bigger] % 2 == 0) {
				even = 1 - even; 
			}
		}
		if(mouse_clicked_2) {
			mouse_clicked_2 = 0; mouse_clicked_1 = 0; 
			int x_1 = floor(mouse_pos_1[0] / square_size); 
			int y_1 = floor(mouse_pos_1[1] / square_size); 
			int x_2 = floor(mouse_pos_2[0] / square_size); 
			int y_2 = floor(mouse_pos_2[1] / square_size); 
			Move move; 
			move.src = x_1+y_1*num[0]; 
			move.dst = x_2+y_2*num[0];
			game.move(move); 
			board = game.board; 
		}			
		window.display();
	}
	return 0; 
}
