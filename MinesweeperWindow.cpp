#include "MinesweeperWindow.h"
#include <stdio.h>
#include <time.h>
#include <thread>
#include <FL/fl_ask.H>

MinesweeperWindow::MinesweeperWindow(Point xy, int width, int height, int mines, const string& title) :
	Graph_lib::Window{xy, width * cellSize, height*cellSize, title}, width{width}, height{height}, mines{mines}
	//Initialiser medlemsvariabler, bruker ogsaa konstruktoren til Windowsklassen
{
	// Legg til alle tiles paa vinduet
	srand (time(NULL));
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int y = i* cellSize,
				x = j * cellSize;
			tiles.push_back(new Tile{ Point{x, y}, cellSize, cb_click, false}); 
			attach(tiles.back());
		}
	}

	//Legg til miner paa tilfeldige posisjoner
	for(int i = 0; i < mines; i++){
		int random = rand() % tiles.size();
		if(tiles[random].getisMine() == false){
			tiles[random].setisMine(true);
		}
	}
	
	// Fjern window reskalering
	resizable(nullptr);
	size_range(x_max(), y_max(), x_max(), y_max());
}

int MinesweeperWindow::countMines(vector<Point> points) const {
	int count = 0;
	Point xy;
	for(int i = 0; i < points.size(); i++){
		xy = points[i];
		if(at(xy).getisMine()){
			count++;
		}
	}
	return count;
};
vector<Point> MinesweeperWindow::adjacentPoints(Point xy) const {
	vector<Point> points;
	for (int di = -1; di <= 1; ++di) {
		for (int dj = -1; dj <= 1; ++dj) {
			if (di == 0 && dj == 0) {
				continue;
			}

			Point neighbour{ xy.x + di * cellSize,xy.y + dj * cellSize };
			if (inRange(neighbour)) {
				points.push_back(neighbour);
			}
		}
	}

	return points;
}

void MinesweeperWindow::win(){
	int counter = 0;
	int tilesclosed = 0;
	bool temp = true;


	while(temp){
		for(int i = 0; i < tiles.size(); i++){
			if(tiles[i].getisMine() && tiles[i].getState() == Cell::flagged){
				counter++;
			}
			if(tiles[i].getState() == Cell::open){
				tilesclosed++;
			}
		}
		if(tiles.size() == tilesclosed + counter){
			cout << "you win " << endl;
			temp = false;
			if(fl_choice("You win", "Ok", 0, 0) == 0){
				exit(0);
			}
		}
		counter = 0;
		tilesclosed = 0;
		this_thread::sleep_for(1s);
	}
}

void MinesweeperWindow::openTile(Point xy) {
	int m = 0;
	int i = 0;
	int timeout = 0;
	bool state = true;
	vector<Point> empty;

	if(at(xy).getState() == Cell::closed && at(xy).getisMine() == false){
		m = countMines(adjacentPoints(xy));
		
		if(!(m == 0)){
			at(xy).setAdjMines(m);
			at(xy).open();
		
		}else{
			empty = adjacentPoints(xy);
			at(xy).open();

			while(state){

				for(int x = 0; x < 280; x++){
					for(int y = 0; y < 280; y++){
						xy.x = x;
						xy.y = y;
						m = countMines(adjacentPoints(xy));

						if(m >= 1){
							if(at(xy).getisMine()){
								state = false;
							}
						}else{
							if(at(xy).getState() == Cell::closed && !(at(xy).getisMine())){
								at(xy).open();
							}
						}
						
					}
				}
				state = false;
				//while tile is empty open if not ignore
				

				/*
				state = (m == 0) && (timeout < 50);
				m = countMines(adjacentPoints(empty[i]));
				
				if(m >= 1){
					at(empty[i]).setAdjMines(m);
					at(empty[i]).open();
					if(at(empty[i]).getisMine()){
						empty.erase(empty.begin() + i - 1);
						state = false;
					}
				
				}else{
					
					if(at(empty[i]).getState() == Cell::closed){
						at(empty[i]).open();
					}
					
					if(i + 1 == empty.size()){
						empty = adjacentPoints(empty[i]);
						i = -1;
					}
					i++;
					timeout++;
				}
				*/
			}
		}			
	
	}else if(at(xy).getisMine()){
		cout << "loose " << endl;
		
		if(fl_choice("You lost", "Ok", 0, 0) == 0){
			exit(0);
		}

	}
}

void MinesweeperWindow::flagTile(Point xy) {
	if(at(xy).getState() == Cell::closed || at(xy).getState() == Cell::flagged){
		at(xy).flag();
	}
}

//Kaller opentile ved venstreklikk og flagTile ved hoyreklikk/trykke med to fingre paa mac
void MinesweeperWindow::cb_click(Address, Address pw)
{
	Point xy{ Fl::event_x(),Fl::event_y() };
	MouseButton mb = static_cast<MouseButton>(Fl::event_button());
	auto& win = reference_to<MinesweeperWindow>(pw);

	if (!win.inRange(xy)) {
		return;
	}

	switch (mb) {
	case MouseButton::left:
		win.openTile(xy);
		break;
	case MouseButton::right:
		win.flagTile(xy);
		break;
	}
	win.flush();
}
